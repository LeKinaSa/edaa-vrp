
#include <string>
#include <iostream>
#include <unordered_set>
#include <tinyxml/tinyxml2.h>
#include "osm.hpp"

using namespace std;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using tinyxml2::XMLError;

OsmXmlData parseOsmXml(const char* path) {
    OsmXmlData data;

    XMLDocument doc;
    doc.LoadFile(path);

    const XMLElement* root = doc.FirstChildElement("osm");
    const XMLElement* bounds = root->FirstChildElement("bounds");

    data.minCoords = Coordinates(
        stod(bounds->Attribute("minlat")),
        stod(bounds->Attribute("minlon"))
    );
    data.maxCoords = Coordinates(
        stod(bounds->Attribute("maxlat")),
        stod(bounds->Attribute("maxlon"))
    );

    const XMLElement* nodeElement = root->FirstChildElement("node");
    while (nodeElement != nullptr) {
        u64 id = stoull(nodeElement->Attribute("id"));
        OsmNode node = { id, Coordinates(
            stod(nodeElement->Attribute("lat")),
            stod(nodeElement->Attribute("lon"))
        ) };
        data.graph.addNode(id, node);

        nodeElement = nodeElement->NextSiblingElement("node");
    }

    static const unordered_set<string> invalidHighwayValues = {
        "pedestrian", "track", "escape", "raceway", "busway", "bus_guideway",
        "footway", "bridleway", "steps", "corridor", "path", "cycleway",
        "proposed", "construction", "elevator"
    };

    const XMLElement* way = root->FirstChildElement("way");
    while (way != nullptr) {
        bool oneWay = false;
        bool valid = true;

        bool highway = false, barrier = false, area = false; // For closed way logic

        const XMLElement* tag = way->FirstChildElement("tag");
        while (tag != nullptr) {
            if (tag->Attribute("k", "highway")) highway = tag->Attribute("v");
            if (tag->Attribute("k", "barrier")) barrier = tag->Attribute("v");
            if (tag->Attribute("k", "area")) area = tag->Attribute("v", "yes");

            if (tag->Attribute("k", "oneway") && tag->Attribute("v", "yes")) {
                oneWay = true;
            }

            if ((tag->Attribute("k", "access") && tag->Attribute("v", "no")) ||
                (tag->Attribute("k", "highway") && invalidHighwayValues.count(tag->Attribute("v")) != 0) ||
                tag->Attribute("k", "waterway")) {
                valid = false;
            }

            tag = tag->NextSiblingElement("tag");
        }

        if (valid) {
            list<u64> wayNodes;

            const XMLElement* n1 = way->FirstChildElement("nd");
            const XMLElement* n2 = n1->NextSiblingElement("nd");

            u64 id1 = stoull(n1->Attribute("ref")), id2;
            wayNodes.push_back(id1);

            while (n2 != nullptr) {
                id2 = stoull(n2->Attribute("ref"));
                wayNodes.push_back(id2);

                const OsmNode& data1 = data.graph.getNode(id1),
                    data2 = data.graph.getNode(id2);

                double weight = data1.coordinates.haversine(data2.coordinates);
                data.graph.addEdge(id1, id2, weight);
                if (!oneWay) {
                    data.graph.addEdge(id2, id1, weight);
                }

                n1 = n2;
                id1 = id2;
                n2 = n2->NextSiblingElement("nd");
            }

            if (wayNodes.front() == wayNodes.back()) {
                if (!(highway || barrier) || area) {
                    for (const auto& id : wayNodes) {
                        data.graph.getNode(id).mapMatch = false;
                    }
                }
            }
        }

        way = way->NextSiblingElement("way");
    }

    // Do not include nodes with degree 0 in map matching
    for (auto& p : data.graph.getNodes()) {
        if (data.graph.getEdges(p.first).empty()) {
            p.second.mapMatch = false;
        }
    }

    return data;
}
