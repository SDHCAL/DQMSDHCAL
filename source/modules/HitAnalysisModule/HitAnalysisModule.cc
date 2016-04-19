/// \file HitAnalysisModule.cc
/*
 *
 * HitAnalysisModule.cc source template automatically generated by a class generator
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


#include "HitAnalysisModule.h"

// -- dqm4hep headers
#include "dqm4hep/DQMMonitorElement.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMModuleApi.h"
#include "dqm4hep/DQMPlugin.h"

//-- lcio headers
#include <EVENT/LCCollection.h>
#include <UTIL/CellIDDecoder.h>

using namespace dqm4hep;

namespace dqm_sdhcal
{

// plugin declaration
DQM_PLUGIN_DECL( HitAnalysisModule, "HitAnalysisModule" )

HitAnalysisModule::HitAnalysisModule() :
	DQMTriventModule(),
	m_moduleLogStr("[HitAnalysisModule]")
{
}

//-------------------------------------------------------------------------------------------------

HitAnalysisModule::~HitAnalysisModule()
{
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode HitAnalysisModule::userReadSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
	m_nActiveLayers = 48;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "NActiveLayers", m_nActiveLayers));

	m_cellIDDecoderString = "M:3,S-1:3,I:9,J:9,K-1:6";
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "CellIDDecoderString", m_cellIDDecoderString));

	/*-----------------------------------------------------*/
	m_clusteringSettings.maxTransversal = 1;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Clustering.MaxTranverseCellID", m_clusteringSettings.maxTransversal));

	m_clusteringSettings.maxLongitudinal = 0;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Clustering.MaxLongitudinalCellID", m_clusteringSettings.maxLongitudinal));

	m_clusteringSettings.useDistanceInsteadCellID = false;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Clustering.UseDistanceInsteadCellID", m_clusteringSettings.useDistanceInsteadCellID));

	m_clusteringSettings.maxTransversalDistance = 11.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Clustering.MaxTranverseDistance", m_clusteringSettings.maxTransversalDistance));

	m_clusteringSettings.maxLongitudinalDistance = 11.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Clustering.MaxLongitudinalDistance", m_clusteringSettings.maxLongitudinalDistance));

	/*-----------------------------------------------------*/
	m_clusteringHelperSettings.longitudinalDistance = 100.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ClusteringHelper.LongitudinalDistanceForIsolation", m_clusteringHelperSettings.longitudinalDistance));

	m_clusteringHelperSettings.transversalDistance = 200.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ClusteringHelper.TransverseDistanceForIsolation", m_clusteringHelperSettings.transversalDistance));

	/*-----------------------------------------------------*/
	// Number of hits in full detector
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::mkdir(this, "/"));
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
	                 "NumberOfHits0", m_pNHit0));
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
	                 "NumberOfHits1", m_pNHit1));
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
	                 "NumberOfHits2", m_pNHit2));
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
	                 "NumberOfHitsTotal", m_pNHit));

	// Number of hits per layer
	for (unsigned int l = 0 ; l < m_nActiveLayers ; l++)
	{
		std::string folderPath = "/Layer" + std::to_string(l);
		if ( dqm4hep::STATUS_CODE_NOT_FOUND == dqm4hep::DQMModuleApi::cd(this, folderPath))
		{
			RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::mkdir(this, folderPath));
			RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::cd(this, folderPath));

			RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			                 "LayerNumberOfHits0", m_layerElementMap[l].m_pNHit0Layer));
			RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			                 "LayerNumberOfHits1", m_layerElementMap[l].m_pNHit1Layer));
			RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			                 "LayerNumberOfHits2", m_layerElementMap[l].m_pNHit2Layer));
			RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			                 "LayerNumberOfHitsTotal", m_layerElementMap[l].m_pNHitTotLayer));
		}
	}

	m_firstLayerCut = 1;// 2;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                 "FirstLayerCut", m_firstLayerCut));

	m_lastLayerCut = 48;//46;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                 "LastLayerCut", m_lastLayerCut));
	m_nMipInLayer = 25;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                 "NMipInLayer", m_nMipInLayer));
	m_nMipMinimum = 40;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                 "NMipMinimum", m_nMipMinimum));

	m_inputCollectionName = "SDHCAL_HIT";
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "InputCollectionName", m_inputCollectionName));

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::userInitModule()
{
	// initialize algorithms
	m_clusteringAlgorithm.SetClusterParameterSetting(m_clusteringSettings);
	m_clusteringHelper.SetClusteringHelperParameterSetting(m_clusteringHelperSettings);
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::processEvent(EVENT::LCEvent *pLCEvent)
{
	LOG4CXX_INFO( dqm4hep::dqmMainLogger , m_moduleLogStr << "Processing physics event no " << pLCEvent->getEventNumber() );
	int nHitProcessedEvent = 0;

	// content management
	caloobject::CaloHitMap caloHitMap;
	caloobject::CaloHitList hits;
	caloobject::CaloClusterList clusters;

	CLHEP::Hep3Vector globalHitShift(0, 0, 0);

	try
	{
		EVENT::LCCollection *pCalorimeterHitCollection = pLCEvent->getCollection(m_inputCollectionName);

		if (NULL == pCalorimeterHitCollection)
			return dqm4hep::STATUS_CODE_SUCCESS;

		UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIDDecoder(m_cellIDDecoderString);

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << "Creating wrapper hits");

		// loop over hits in this event
		for (unsigned int h = 0 ; h < pCalorimeterHitCollection->getNumberOfElements() ; h++)
		{
			EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pCalorimeterHitCollection->getElementAt(h));

			if (NULL == pCaloHit)
				continue;

			int cellID[3];
			cellID[0] = cellIDDecoder(pCaloHit)["I"];
			cellID[1] = cellIDDecoder(pCaloHit)["J"];
			cellID[2] = cellIDDecoder(pCaloHit)["K-1"];

			if ( cellID[2] >= m_nActiveLayers )
			{
				LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Wrong number of layer in your configuration file!");
				LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Found a hit in layer " << cellID[2] << " - Last layer in xml configuration file is " << m_nActiveLayers);
				continue;
			}

			CLHEP::Hep3Vector positionVector(
			  pCaloHit->getPosition()[0],
			  pCaloHit->getPosition()[1],
			  pCaloHit->getPosition()[2] );

			caloobject::CaloHit *pWrapperHit = new caloobject::CaloHit(
			  cellID,
			  positionVector,
			  pCaloHit->getEnergy(),
			  pCaloHit->getTime(),
			  globalHitShift);

			caloHitMap[ cellID[2] ].push_back(pWrapperHit);
			hits.push_back(pWrapperHit);
		}

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << "Creating intra layer clusters");

		for (caloobject::CaloHitMap::iterator iter = caloHitMap.begin(), endIter = caloHitMap.end() ;
		     iter != endIter ; ++iter)
			m_clusteringAlgorithm.Run(iter->second, clusters);

		std::sort(clusters.begin(), clusters.end(), algorithm::ClusteringHelper::SortClusterByLayer);

		int nHit0 = 0;
		int nHit1 = 0;
		int nHit2 = 0;
		int nMip = 0;
		int nHit0Layer = 0;
		int nHit1Layer = 0;
		int nHit2Layer = 0;
		int nMipLayer  = 0;

		for (caloobject::CaloClusterList::const_iterator clusterIter = clusters.begin(), clusterEndIter = clusters.end(); clusterEndIter != clusterIter ; ++clusterIter)
		{
			unsigned int nHitProcessed = 0; 										// Number of hit processed in the current cluster

			int layerId = (*clusterIter)->getLayerID();
			if (layerId >= m_nActiveLayers)
				continue;

			for (caloobject::CaloHitList::const_iterator hitIter = (*clusterIter)->getHits().begin(), hitEndIter = (*clusterIter)->getHits().end(); hitEndIter != hitIter; ++hitIter)
			{
				uint32_t hitWeight = 0;
				int hitThreshold = (*hitIter)->getEnergy();

				if (hitThreshold == 1) {hitWeight = 1; nHit0Layer++;}
				else if (hitThreshold == 2) {hitWeight = 3; nHit1Layer++;}
				else if (hitThreshold == 3) {hitWeight = 15; nHit2Layer++;}
				else
				{
					LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " Wait what? Hit with threshold : " << hitThreshold << " in layer " << layerId);
					continue;
				}

				nMipLayer += hitWeight;
				nHitProcessedEvent++; 	// Total number of hits processed in current event
				nHitProcessed++;			  // Total number of hits processed in current cluster

				// if (hitEndIter == hitIter + 1)
				// {
				// 	int numberOfHit = (*clusterIter)->getHits().size(); // Number of hits in the current cluster
				// 	LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " Processed : " << nHitProcessed << "/" << numberOfHit << " hits in current cluster of layer " << layerId << "\t nHitTotLayer : " << nHit0Layer + nHit1Layer + nHit2Layer  << "\t nHitLayer0 : " << nHit0Layer << "\t nHitLayer1 : " << nHit1Layer << "\t nHitLayer2 : " << nHit2Layer);
				// }
			}

			// uint32_t firstLayer = m_nActiveLayers, lastLayer = 1;
			// for (uint32_t i = 1; i < m_nActiveLayers + 1; ++i)
			// {
			// 	if (nMipLayer > m_nMipInLayer && i < firstLayer) firstLayer = i;
			// 	if (nMipLayer > m_nMipInLayer && i > lastLayer) lastLayer = i;
			// }

			// if (firstLayer >= m_firstLayerCut && lastLayer <= m_lastLayerCut)
			// {
			// 	for (uint32_t i = 0; i < m_nActiveLayers; ++i)
			// 	{
			// 		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " Adding hits in layer : " << i << "\t hits0 : " << nHit0Layer << "\t hits1 : " << nHit1Layer<< "\t hits2 : " << nHit2Layer);

			// Stack hits from multiple clusters in the same layer before filling histograms
			caloobject::CaloClusterList::const_iterator nextClusterIter;
			if (clusterEndIter != clusterIter + 1)
				nextClusterIter = std::next(clusterIter, 1);
			else
				nextClusterIter = clusterIter;

			if ( (*nextClusterIter)->getLayerID() != layerId || nextClusterIter == clusterIter)
			{
				if (0 != nHit0Layer)
					m_layerElementMap[layerId].m_pNHit0Layer->get<TH1>()->Fill(nHit0Layer);
				if (0 != nHit1Layer)
					m_layerElementMap[layerId].m_pNHit1Layer->get<TH1>()->Fill(nHit1Layer);
				if (0 != nHit2Layer)
					m_layerElementMap[layerId].m_pNHit2Layer->get<TH1>()->Fill(nHit2Layer);

				m_layerElementMap[layerId].m_pNHitTotLayer->get<TH1>()->Fill(nHit0Layer + nHit1Layer + nHit2Layer);

				nHit0 += nHit0Layer;
				nHit1 += nHit1Layer;
				nHit2 += nHit2Layer;
				nMip += nMipLayer;
				nHit0Layer = 0;
				nHit1Layer = 0;
				nHit2Layer = 0;
			}
			// 	}
			// }
		} // Loop on TrackingClusters

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " Total hits processed : " << nHitProcessedEvent << "\t hits(0+1+2) : " << nHit0 + nHit1 + nHit2 << "\t hits0 : " << nHit0 << "\t hits1 : " << nHit1 << "\t hits2 : " << nHit2 << "\t nMip : " << nMip);

		if (nMip > m_nMipMinimum)
		{
			m_pNHit0->get<TH1>()->Fill(nHit0);
			m_pNHit1->get<TH1>()->Fill(nHit1);
			m_pNHit2->get<TH1>()->Fill(nHit2);
			m_pNHit->get<TH1>()->Fill(nHit0 + nHit1 + nHit2);
		}
	}
	catch (EVENT::DataNotAvailableException &exception)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << "Caught EVENT::DataNotAvailableException : " << exception.what());
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger ,  m_moduleLogStr << "Skipping event" );
		this->clearEventContents(hits, clusters);
		return STATUS_CODE_SUCCESS;
	}
	catch (...)
	{
		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger ,  m_moduleLogStr << "Caught unknown exception !");
		this->clearEventContents(hits, clusters);
		return STATUS_CODE_FAILURE;
	}

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::startOfCycle()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::endOfCycle()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::startOfRun(DQMRun * pRun)
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::endOfRun(DQMRun * pRun)
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode HitAnalysisModule::endModule()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

void HitAnalysisModule::clearEventContents(caloobject::CaloHitList & hits, caloobject::CaloClusterList & clusters)
{
	for_each(hits.begin(), hits.end(), [] (caloobject::CaloHit * pCaloHit) { delete pCaloHit; });
	for_each(clusters.begin(), clusters.end(), [] (caloobject::CaloCluster * pCluster) { delete pCluster; });

	hits.clear();
	clusters.clear();
}

//-------------------------------------------------------------------------------------------------

void HitAnalysisModule::resetElements()
{
	for (std::map<unsigned int, LayerElements>::iterator iter = m_layerElementMap.begin(), endIter = m_layerElementMap.end() ;
	     endIter != iter ; ++iter)
	{
		iter->second.m_pNHit0Layer->reset();
		iter->second.m_pNHit1Layer->reset();
		iter->second.m_pNHit2Layer->reset();
		iter->second.m_pNHitTotLayer->reset();
	}
	m_pNHit0->reset();
	m_pNHit1->reset();
	m_pNHit2->reset();
	m_pNHit->reset();
}

}

