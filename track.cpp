/*
* Created  by rbh, 30/01/11
* Modified by nas, 21/12/16
*/

#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>

#include "../headers/utilities.h"
#include "../headers/xmlparser.h"
#include "../headers/track.h"

namespace GPXP
{
	Track::Track(string source, bool isFileName)
	{
		using namespace XML_Parser;
		using std::endl;

		variables structVar;		//struct class
		checkSource(isFileName, source, structVar.oss);		//check if file is valid
		
		elementNotFound(source, "gpx", structVar.oss);		//check if tag exist
		elementNotFound(source, "trk", structVar.oss);		//check if tag exist
		

		structVar.temp = getElement(source, "trk");
		getTagName(source, structVar.temp, structVar.oss, "track");		//get Name of track
		
		mergeTrkSegs(source, structVar);
		checkMergedTrkSegs(structVar.mergedTrkSegs, source);
		elementNotFound(source, "trkpt", structVar.oss);		//check if tag exist
													
		structVar.temp = getAndEraseElement(source, "trkpt");
		structVar.lat = getElementAttribute(structVar.temp, "lat");
		structVar.lon = getElementAttribute(structVar.temp, "lon");
		structVar.temp = getElementContent(structVar.temp);
		structVar.temp2 = getElement(structVar.temp, "ele");
		structVar.ele = getElementContent(structVar.temp2);
		structVar.temp2 = getElement(structVar.temp, "time");
		structVar.time = getElementContent(structVar.temp2);
		structVar.name;
		elementFound(structVar.temp, structVar.temp2, structVar.name);		//check if tag exist
											
		Position currentPos(structVar.lat, structVar.lon, structVar.ele);
		addStartPosition(currentPos, structVar.name, structVar.oss);		//push Postion element
													
		seconds startTime = timeFromString(structVar.time);
		arrived.push_back(0);
		departed.push_back(0);

		getPositionTrack(source, structVar, currentPos, startTime);

		vector<Position>::const_iterator pci1, pci2;
		getPosition(pci1, pci2, structVar.oss);	

		return;
	}

	void Track::mergeTrkSegs(std::string &source, variables &use)
	{
		using namespace XML_Parser;
		while (elementExists(source, "trkseg"))
		{
			use.temp = getAndEraseElement(source, "trkseg");
			use.trkseg = getElementContent(use.temp);
			use.segname = getAndEraseElement(use.trkseg, "name");
			use.mergedTrkSegs += use.trkseg;
		}
	}

	void Track::getPositionTrack(std::string &source, variables &use, GPXP::Position &currentPos, const GPXP::seconds &startTime)
	{
		using namespace XML_Parser;
		while (elementExists(source, "trkpt"))
		{
			use.temp = getAndEraseElement(source, "trkpt");
			use.lat = getElementAttribute(use.temp, "lat");
			use.lon = getElementAttribute(use.temp, "lon");
			use.temp = getElementContent(use.temp);
			use.temp2 = getElement(use.temp, "ele");
			use.ele = getElementContent(use.temp2);
			use.temp2 = getElement(use.temp, "time");
			use.time = getElementContent(use.temp2);
			Position nextPos(use.lat, use.lon, use.ele);

			if (areSameLocation(currentPos, nextPos))
			{
				// If we're still at the same location, then we haven't departed yet.
				departed.back() = timeFromString(use.time) - startTime;
				use.oss << std::endl << "position ignored: " << nextPos.toString();
			}
			else
			{
				elementFound(use.temp, use.temp2, use.name);

				positions.push_back(nextPos);
				positionNames.push_back(use.name);
				seconds timeElapsed = timeFromString(use.time) - startTime;
				arrived.push_back(timeElapsed);
				departed.push_back(timeElapsed);
				use.oss << std::endl << "position added: " << nextPos.toString();
				use.oss << " at time " << timeAsString(departed.back());
				currentPos = nextPos;
			}
		}
		use.oss << std::endl << positions.size() << " positions added";
	}

	void Track::checkMergedTrkSegs(std::string &mergedTrkSegs, std::string &source)
	{
		if (!mergedTrkSegs.empty())
		{
			source = mergedTrkSegs;
		}
	}

	seconds Track::totalTime() const
	{
		/* Computes total elapsed time between start and finish of the Track.
		* One of the 3 functions should exploit:
		* totalTime() == restingTime() + travellingTime()
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	seconds Track::restingTime() const
	{
		/* Computes total elapsed time between start and finish of the Track
		* that is spent not moving.
		* One of the 3 functions should exploit:
		* totalTime() == restingTime() + travellingTime()
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	seconds Track::travellingTime() const
	{
		/* Total elapsed time between start and finish of the Track that is
		* spent moving.
		* One of the 3 functions should exploit:
		* totalTime() == restingTime() + travellingTime()
		*/
		bool implemented = false;
		assert(implemented);
		return (0);
	}

	speed Track::maxSpeed() const
	{
		/* Computes the greatest speed between any 2 Positions in the Track.
		*/
		bool implemented = false;
		assert(implemented);
		return (0.0);
	}

	speed Track::averageSpeed(bool includeRests) const
	{
		/* Computes the average speed over the track.
		* The parameter determines whether the divisor is the totalTime()
		* or the travellingTime().
		*/
		bool implemented = false;
		assert(implemented);
		return (0.0);
	}

	speed Track::maxRateOfClimb() const
	{
		/* Computes the greatest rate of climb between any 2 Positions.
		*/
		bool implemented = false;
		assert(implemented);
		return (0.0);
	}

	speed Track::maxRateOfDescent() const
	{
		/* Computes the greatest rate of descent between any 2 Positions.
		*/
		bool implemented = false;
		assert(implemented);
		return (0.0);
	}
}