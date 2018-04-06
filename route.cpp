/*
* Created  by rbh, 27/02/15
* Modified by nas, 18/12/16
*/

#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>

#include "../headers/xmlparser.h"
#include "../headers/route.h"



namespace GPXP
{

	const metres Route::granularity = 20;
	const Position Route::northPole = Position(90, 0, 0);


	Route::Route(string source, bool isFileName)
	{
		using namespace XML_Parser;
		using std::endl;

		variables structVar; //struct class

		checkSource(isFileName, source, structVar.oss);			 //check if file is valid 

		elementNotFound(source, "gpx" , structVar.oss);				//check if tag exist

		elementNotFound(source, "rte", structVar.oss);

		structVar.temp = getElement(source, "rte");

		getTagName(source, structVar.temp, structVar.oss,"route");		//get Name of route

		elementNotFound(source, "rtept", structVar.oss);

		structVar.temp = getAndEraseElement(source, "rtept");
		structVar.lat = getElementAttribute(structVar.temp, "lat");
		structVar.lon = getElementAttribute(structVar.temp, "lon");
		structVar.temp = getElementContent(structVar.temp);
		structVar.temp2 = getElement(structVar.temp, "ele");
		structVar.ele = getElementContent(structVar.temp2);

		elementFound(structVar.temp, structVar.temp2, structVar.name);

		Position currentPos(structVar.lat, structVar.lon, structVar.ele);

		addStartPosition(currentPos, structVar.name, structVar.oss);	//push Postion element

		displayPositionSize(source, structVar, currentPos);

		vector<Position>::const_iterator pci1, pci2;
		getPosition(pci1, pci2, structVar.oss);
		return;
	}

	void Route::displayPositionSize(std::string &source, variables &use, GPXP::Position &currentPos)
	{
		using namespace XML_Parser;
		while (elementExists(source, "rtept"))
		{
			use.temp = getAndEraseElement(source, "rtept");
			use.lat = getElementAttribute(use.temp, "lat");
			use.lon = getElementAttribute(use.temp, "lon");
			use.temp = getElementContent(use.temp);
			use.ele = getElementContent(getElement(use.temp, "ele"));

			elementFound(use.temp, use.temp2, use.name);

			Position nextPos(use.lat, use.lon, use.ele);
			addPosition(currentPos, nextPos, use.oss, use.name);
		}
		use.oss << std::endl << positions.size() << " positions added";
	}

	bool Route::getPosition(std::vector<GPXP::Position>::const_iterator &pci1, std::vector<GPXP::Position>::const_iterator &pci2, std::ostringstream &oss)
	{
		routeLength = 0;

		pci1 = pci2 = positions.begin();
		++pci2;

		for (pci2; pci2 != positions.end(); ++pci1, ++pci2)
		{
			metres deltaH = distanceBetween(*pci1, *pci2);
			metres deltaV = pci1->getElevation() - pci2->getElevation();
			routeLength += std::sqrt(std::pow(deltaH, 2) + std::pow(deltaV, 2));
		}

		constructorReport = oss.str();
		return constructorSucceeded = true;
	}


	void Route::addPosition(Position &currentPos, Position &nextPos, std::ostringstream &oss, std::string &name)
	{
		if (distanceBetween(currentPos, nextPos) < granularity)
		{
			oss << std::endl << "position ignored: " << nextPos.toString();
		}
		else
		{
			positions.push_back(nextPos);
			positionNames.push_back(name);
			oss << std::endl << "position added: " << nextPos.toString();
			currentPos = nextPos;
		}
	}

	void Route::elementFound(std::string &temp, std::string &temp2, std::string &name)
	{
		using namespace XML_Parser;
		if (elementExists(temp, "name"))
		{
			temp2 = getElement(temp, "name");
			name = getElementContent(temp2);
		}
		else
		{
			name = "";
		}
	}

	void Route::addStartPosition(GPXP::Position currentPos, std::string name, std::ostringstream &oss)
	{
		positions.push_back(currentPos);		
		positionNames.push_back(name);
		oss << std::endl << "start position added: " << currentPos.toString();
	}

	void Route::getTagName(std::string &source, std::string &temp, std::ostringstream &oss,std::string type)
	{
		using namespace XML_Parser;

		source = getElementContent(temp);	//getElement Tag name

		if (elementExists(source, "name"))	//if source is exist, display the type and file name
		{
			temp = getAndEraseElement(source, "name");
			routeName = getElementContent(temp);
			oss <<  std::endl <<  type << " name is " << routeName;
		}
	}

	bool Route::elementNotFound(std::string &source, std::string type,std::ostringstream &oss)
	{
		using namespace XML_Parser;	
		if (!elementExists(source, type))	//if elements is no exist in source file
		{
			oss << std::endl << "no " << type << " tag";
			constructorReport = oss.str();	  //returns a string object with a copy of the current contents of the stream
			return constructorSucceeded = false;		
		}
	}

	bool Route::checkSource(bool isFileName, std::string &source, std::ostringstream &oss)
	{
		bool retflag = false;
		if (isFileName)			
		{
			std::ifstream file(source);

			if (!file)			//if source file does not exist
			{
				oss << std::endl << "error opening constructor source file " << source;
				constructorReport = oss.str();		////returns a string object with a copy of the current contents of the stream
				return constructorSucceeded = false;
			}
			else
			{
				oss << std::endl << "source file " << source << " opened okay";
				string content, line;
				while (file.good())		//file is able to read
				{
					getline(file, line);
					content += '\n';
					content += line;
				}
				oss << std::endl << "source file " << source << " read okay";
				source = content;
				return retflag = true;
			}
		}	
	}


	bool Route::builtOk() const
	{
		return constructorSucceeded;
	}

	string Route::buildReport() const
	{
		/* Returns the report generated by the constructor, for debugging purposes.
		*/
		bool implemented = true;
		assert(implemented);
		return constructorReport;
	}

	string Route::name() const
	{
		/* Returns the name of the Route, or "Unnamed Route" if nameless.
		*/
		bool implemented = true;
		assert(implemented);
		return routeName.empty() ? "Unnamed Route" : routeName;
	}

	metres Route::totalLength() const
	{
		/* Computes and returns the total length of the Route; this is
		* the sum of the distances between the successive Positions of the Route.
		*/
		bool implemented = true;
		assert(constructorSucceeded);
		assert(implemented);
		return routeLength;
	}

	metres Route::netLength() const
	{
		/* Returns the distance between the first and last points on the Route.
		*/
		bool implemented = true;
		assert(constructorSucceeded);
		assert(implemented);

		const Position & start = positions.front();
		const Position & finish = positions.back();

		metres deltaH = distanceBetween(start, finish);
		metres deltaV = start.getElevation() - finish.getElevation();
		return std::sqrt(std::pow(deltaH, 2) + std::pow(deltaV, 2));
	}

	metres Route::totalHeightGain() const
	{
		/* Returns the sum of all the positive (>0) height differences between points.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	metres Route::netHeightGain() const
	{
		/* Returns the difference in elevation between the first and last points on the
		* Route.
		* Should be negative if there was a net height loss.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	degrees Route::maxUphillGradient() const
	{
		/* Returns the maximum value of gradient between successive points on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	degrees Route::maxDownhillGradient() const
	{
		/* Returns the minimum (most negative) value of gradient between successive points
		* on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	degrees Route::minLat() const
	{
		/* Returns the latitude of the most Southerly point on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (90.0);
	}

	degrees Route::maxLat() const
	{
		/* Returns the latitude of the most Northerly point on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (-90.0);
	}

	degrees Route::minLon() const
	{
		/* Returns the longitude of the most Westerly point on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (90.0);
	}

	degrees Route::maxLon() const
	{
		/* Returns the longitude of the most Easterly point on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (-90.0);
	}

	metres Route::minEle() const
	{
		/* Returns the elevation of the lowest point on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (4000);
	}

	metres Route::maxEle() const
	{
		/* Returns the elevation of the highest point on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	unsigned int Route::timesVisited(const Position & pos) const
	{
		/* Returns the number of times that the Position is visited on the Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	unsigned int Route::timesVisited(const string & nameTag) const
	{
		/* Returns the number of times that the Position with <name>nameTag</name>
		* is visited on the Route.  If nameTag == "", return 0.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	unsigned int Route::numPositions() const
	{
		/* Returns the number of Positions in this Route.
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	string Route::nameOf(const Position & pos) const
	{
		/* Determine whether the Position occurs anywhere on the Route,
		* and if so whether it has a name at any of the occurrences;
		* return the name if it has one, or an empty string otherwise.
		*/
		bool implemented = false;
		assert(implemented);
		return (string("not implemented"));
	}

	Position Route::positionOf(const string & nameTag) const
	{
		/* This function is the inverse of Route::nameOf().  It searches on the Route
		* for a point named nameTag.  If found, its Position is returned, else the
		* North Pole Position is returned.
		*/
		bool implemented = false;
		assert(implemented);
		return (northPole);
	}

	const Position & Route::operator[](unsigned int idx) const
	{
		/* Provides access to the sequence of points that make up the Route.
		* Out-of-range access should return the North Pole Position.
		*/
		bool implemented = false;
		assert(implemented);
		return (northPole);
	}

	bool Route::areSameLocation(const Position & p1, const Position & p2) const
	{
		/* Determines whether the 2 Positions parameters are less than "granularity" apart.
		*/
		bool implemented = true;
		assert(implemented);
		return (distanceBetween(p1, p2) < granularity);
	}

	bool Route::append(const Position & pos, const string & name)
	{
		/*  Append the position to the end of the "positions" vector, unless
		*  it is close enough to the last Position to be considered the same
		*  location (see "areSameLocation()").  The routeLength is updated
		*  accordingly.
		*/
		bool implemented = true;
		assert(implemented);
		Position prevPos = positions.back();
		if (areSameLocation(pos, prevPos))
		{
			return false;
		}
		else
		{
			metres deltaH = distanceBetween(pos, prevPos);
			metres deltaV = pos.getElevation() - prevPos.getElevation();
			positions.push_back(pos);
			positionNames.push_back(name);
			routeLength += std::sqrt(std::pow(deltaH, 2) + std::pow(deltaV, 2));
			return true;
		}
	}

	void Route::simplify(unsigned int overridingGranularity)
	{
		/* Simplify the Route by reducing the number of points: any point in
		* the Route that differs in distance from its predecessor by less than
		* the parameter is discarded.
		*/
		bool implemented = false;
		assert(implemented);
	}
}
