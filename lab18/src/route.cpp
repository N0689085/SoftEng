#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "xmlparser.h"
#include "geometry.h"
#include "earth.h"
#include "route.h"

using namespace GPS;

std::string Route::name() const
{
   return routeName.empty() ? "Unnamed Route" : routeName;
}

unsigned int Route::numPositions() const
{
    // return positions.size();
    int numOfPositions,
    for (int i = 1;  i < numPositions(); i++) int i = numOfPositions;
    return numOfPositions;
}

metres Route::totalLength() const
{
    return routeLength;
}

metres Route::netLength() const
{
    metres delatH, deltaV;
    deltaH = distanceBetween(positions[0], positions[numPositions()-1]);
    deltaV = positions[0].elevation() - positions[numPositions()-1].elevation();
    return sqrt(pow(deltaH,2) + pow(deltaV,2));
}

metres Route::totalHeightGain() const
{
    metres totHeight = 0;
    for (int i = 0; i < numPositions(); i++) {
        metres diffHeight = positions[i+1].elevation() - positions[i].elevation();
        if (i == numPositions()-1) break;
        if (diffHeight > 0) totHeight += diffHeight;
    }
    return totHeight;
}

metres Route::netHeightGain() const
{
    metres netHeight = positions[numPositions()-1].elevation() - positions[0].elevation();
    if (netHeight > 0) return netHeight;
    return 0;
}

degrees Route::minLatitude() const
{
    degrees minLatitude = positions[0].latitude();
    for (int i = 1; 1 < numPositions(); i++) {
        if (position[i].latitude() < minLatitude) minLatitude = position[i].latitude();
    }
    return minLatitude;
}

degrees Route::maxLatitude() const
{
    degrees maxLatitude = positions[0].latitude();
    for (int i = 1; 1 < numPositions(); i++) {
        if (position[i].latitude() > maxLatitude) maxLatitude = position[i].latitude();
    }
    return maxLatitude;
}

degrees Route::minLongitude() const
{
    degrees minLongitude = positions[0].longitude();
    for (int i = 1; 1 < numPositions(); i++) {
        if (position[i].longitude() < minLongitude) minLongitude = position[i].longitude();
    }
    return minLongitude;
}

degrees Route::maxLongitude() const
{
    degrees maxLongitude = positions[0].longitude();
    for (int i = 1; 1 < numPositions(); i++) {
        if (position[i].longitude() > maxLongitude) maxLongitude = position[i].longitude();
    }
    return maxLongitude;
}

metres Route::minElevation() const
{
    metres lowestElevation = positions[0].elevation();
    for (int i = 1; i < numPositions(); i++) {
        if (positions[i].elevation() < lowestElevation) lowestElevation = positions[i].elevation();
    }
    return lowestElevation;
}

metres Route::maxElevation() const
{
    metres highestElevation = positions[0].elevation();
    for (int i = 1; i < numPositions(); i++) {
        if (positions[i].elevation() > lowestElevation) highestElevation = positions[i].elevation();
    }
    return highestElevation;
}

degrees Route::maxGradient() const
{
    const bool implemented = false;
    assert(implemented);
}

degrees Route::minGradient() const
{
    const bool implemented = false;
    assert(implemented);
}

degrees Route::steepestGradient() const
{
    const bool implemented = false;
    assert(implemented);
}

Position Route::operator[](unsigned int idx) const
{
    if (idx < numPositions() && idx >= 0) {
        return positions[idx];
    } else {
        throw std::out_of_range;
    }
}

Position Route::findPosition(const std::string & soughtName) const
{
    bool validName = false;
    for (unsigned int i = 0; i < positionNames.size(); i++) {
        if (soughtName == positionNames[i]) {
            validName = true;
            return positions[i];
        }
    }
    if (!validName) throw std::invalid_argument("Position Name not found");
}

std::string Route::findNameOf(const Position & soughtPos) const
{
    const bool implemented = false;
    assert(implemented);
}

unsigned int Route::timesVisited(const std::string & soughtName) const
{
    const bool implemented = false;
    assert(implemented);
}

unsigned int Route::timesVisited(const Position & soughtPos) const
{
    const bool implemented = false;
    assert(implemented);
}

std::string Route::buildReport() const
{
    return report;
}

Route::Route(std::string source, bool isFileName, metres granularity)
{
    using namespace std;
    using namespace XML::Parser;
    string lat,lon,ele,name,temp,temp2;
    metres deltaH,deltaV;
    ostringstream oss,oss2;
    unsigned int num;
    this->granularity = granularity;
    if (isFileName){
        ifstream fs = ifstream(source);
        if (! fs.good()) throw invalid_argument("Error opening source file '" + source + "'.");
        oss << "Source file '" << source << "' opened okay." << endl;
        while (fs.good()) {
            getline(fs, temp);
            oss2 << temp << endl;
        }
        source = oss2.str();
    }
    if (! elementExists(source,"gpx")) throw domain_error("No 'gpx' element.");
    temp = getElement(source, "gpx");
    source = getElementContent(temp);
    if (! elementExists(source,"rte")) throw domain_error("No 'rte' element.");
    temp = getElement(source, "rte");
    source = getElementContent(temp);
    if (elementExists(source, "name")) {
        temp = getAndEraseElement(source, "name");
        routeName = getElementContent(temp);
        oss << "Route name is: " << routeName << endl;
    }
    num = 0;
    if (! elementExists(source,"rtept")) throw domain_error("No 'rtept' element.");
    temp = getAndEraseElement(source, "rtept");
    if (! attributeExists(temp,"lat")) throw domain_error("No 'lat' attribute.");
    if (! attributeExists(temp,"lon")) throw domain_error("No 'lon' attribute.");
    lat = getElementAttribute(temp, "lat");
    lon = getElementAttribute(temp, "lon");
    temp = getElementContent(temp);
    if (elementExists(temp, "ele")) {
        temp2 = getElement(temp, "ele");
        ele = getElementContent(temp2);
        Position startPos = Position(lat,lon,ele);
        positions.push_back(startPos);
        oss << "Position added: " << startPos.toString() << endl;
        ++num;
    } else {
        Position startPos = Position(lat,lon);
        positions.push_back(startPos);
        oss << "Position added: " << startPos.toString() << endl;
        ++num;
    }
    if (elementExists(temp,"name")) {
        temp2 = getElement(temp,"name");
        name = getElementContent(temp2);
    }
    positionNames.push_back(name);
    Position prevPos = positions.back(), nextPos = positions.back();
    while (elementExists(source, "rtept")) {
        temp = getAndEraseElement(source, "rtept");
        if (! attributeExists(temp,"lat")) throw domain_error("No 'lat' attribute.");
        if (! attributeExists(temp,"lon")) throw domain_error("No 'lon' attribute.");
        lat = getElementAttribute(temp, "lat");
        lon = getElementAttribute(temp, "lon");
        temp = getElementContent(temp);
        if (elementExists(temp, "ele")) {
            temp2 = getElement(temp, "ele");
            ele = getElementContent(temp2);
            nextPos = Position(lat,lon,ele);
        } else nextPos = Position(lat,lon);
        if (areSameLocation(nextPos, prevPos)) oss << "Position ignored: " << nextPos.toString() << endl;
        else {
            if (elementExists(temp,"name")) {
                temp2 = getElement(temp,"name");
                name = getElementContent(temp2);
            } else name = ""; // Fixed bug by adding this.
            positions.push_back(nextPos);
            positionNames.push_back(name);
            oss << "Position added: " << nextPos.toString() << endl;
            ++num;
            prevPos = nextPos;
        }
    }
    oss << num << " positions added." << endl;
    routeLength = 0;
    for (unsigned int i = 1; i < num; ++i ) {
        deltaH = distanceBetween(positions[i-1], positions[i]);
        deltaV = positions[i-1].elevation() - positions[i].elevation();
        routeLength += sqrt(pow(deltaH,2) + pow(deltaV,2));
    }
    report = oss.str();
}

void Route::setGranularity(metres granularity)
{
    bool implemented = false;
    assert(implemented);
}

bool Route::areSameLocation(const Position & p1, const Position & p2) const
{
    return (distanceBetween(p1,p2) < granularity);
}
