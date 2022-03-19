
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

    return data;
}
