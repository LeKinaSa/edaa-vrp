
#include <string>
#include <iostream>
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
        OsmNode node = { Coordinates(
            stod(nodeElement->Attribute("lat")),
            stod(nodeElement->Attribute("lon"))
        ) };
        data.graph.addNode(stoull(nodeElement->Attribute("id")), node);

        nodeElement = nodeElement->NextSiblingElement("node");
    }

    const XMLElement* way = root->FirstChildElement("way");
    while (way != nullptr) {
        bool oneWay = true;

        const XMLElement* tag = way->FirstChildElement("tag");
        while (tag != nullptr) {
            if (tag->Attribute("k", "oneway")) {
                if (tag->Attribute("v", "no")) {
                    oneWay = false;
                }
                break;
            }

            tag = tag->NextSiblingElement("tag");
        }

        const XMLElement* n1 = way->FirstChildElement("nd");
        const XMLElement* n2 = n1->NextSiblingElement("nd");

        while (n2 != nullptr) {
            u64 id1 = stoull(n1->Attribute("ref")), 
                id2 = stoull(n2->Attribute("ref"));

            const OsmNode& data1 = data.graph.getNode(id1),
                data2 = data.graph.getNode(id2);

            double weight = data1.coordinates.haversine(data2.coordinates);
            data.graph.addEdge(id1, id2, weight);
            if (!oneWay) {
                data.graph.addEdge(id2, id1, weight);
            }

            n1 = n2;
            n2 = n2->NextSiblingElement("nd");
        }

        way = way->NextSiblingElement("way");
    }

    return data;
}
