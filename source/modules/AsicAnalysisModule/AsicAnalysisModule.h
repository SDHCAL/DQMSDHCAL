  /// \file AsicAnalysisModule.h
/*
 *
 * AsicAnalysisModule.h header template automatically generated by a class generator
 * Creation date : ven. ao�t 28 2015
 *
 * This file is part of DQMSDHCAL libraries.
 * 
 * DQMSDHCAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * DQMSDHCAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DQMSDHCAL.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete, Arnaud Steen
 * @copyright CNRS , IPNL
 */


#ifndef ASICANALYSISMODULE_H
#define ASICANALYSISMODULE_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"

// -- lcio headers
#include "lcio.h"
#include "EVENT/CalorimeterHit.h"

// -- specific module header
#include "Asic.h"
#include "Cluster.h"
#include "PCA.hh"
#include "Linear3DFit.hh"
#include "ThreeVector.hh"
#include "Layer.h"

// -- std headers
#include <string>
#include <cstring>
#include <vector>

#include "DQMTriventModule.h"

namespace dqm_sdhcal
{

/** AsicAnalysisModule class
 */ 
class AsicAnalysisModule : public DQMTriventModule
{
public:
	/** Constructor
	 */
	AsicAnalysisModule();

	/** Destructor
	 */
	virtual ~AsicAnalysisModule();

	dqm4hep::StatusCode userInitModule();
	dqm4hep::StatusCode userReadSettings(const dqm4hep::TiXmlHandle xmlHandle);
	dqm4hep::StatusCode processNoisyEvent(EVENT::LCEvent *pLCEvent);
	dqm4hep::StatusCode processPhysicalEvent(EVENT::LCEvent *pLCEvent);

	dqm4hep::StatusCode startOfCycle();
	dqm4hep::StatusCode endOfCycle();
	dqm4hep::StatusCode startOfRun(dqm4hep::DQMRun *const pRun);
	dqm4hep::StatusCode endOfRun(dqm4hep::DQMRun *const pRun);
	dqm4hep::StatusCode endModule();

	dqm4hep::StatusCode doTrackStudy(const std::vector<EVENT::LCEvent*> &eventList);
	void doTrackStudy();
	void clearContents();
	bool TrackSelection(std::vector<Cluster*> &clVec);
	void LayerProperties(std::vector<Cluster*> &clVec);
	int findAsicKey(int layer,float x, float y);

	void resetElements();

private:
	struct LayerElements
	{
		dqm4hep::DQMMonitorElement *m_pEfficiencyMap;
		dqm4hep::DQMMonitorElement *m_pMultiplicityMap;
	};

	unsigned int                    m_nActiveLayers;
	std::string                     m_inputCollectionName;

	std::vector<EVENT::CalorimeterHit*> m_calorimeterHitCollection;

	std::vector<Cluster*> clusters;
	std::map<int,Asic*> asicMap;



	std::map<unsigned int, LayerElements>     m_layerElementMap;

	dqm4hep::DQMMonitorElement               *m_pLayerEfficiency;
	dqm4hep::DQMMonitorElement               *m_pLayerMultiplicity;
	dqm4hep::DQMMonitorElement               *m_pAsicEfficiency;
	dqm4hep::DQMMonitorElement               *m_pAsicMultiplicity;
	dqm4hep::DQMMonitorElement               *m_pStackedEfficiencyMap;
	dqm4hep::DQMMonitorElement               *m_pStackedMultiplicityMap;
	dqm4hep::DQMMonitorElement               *m_pGlobalEfficiency;
	dqm4hep::DQMMonitorElement               *m_pGlobalMultiplicity;
	dqm4hep::DQMMonitorElement               *m_pNTracksPerAsic;
}; 

} 

#endif  //  ASICANALYSISMODULE_H
