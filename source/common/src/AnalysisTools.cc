/// \file AnalysisTools.cc
/*
 *
 * AnalysisTools.cc source template automatically generated by a class generator
 * Creation date : ven. avr. 15 2016
 *
 * This file is part of DQMSHCAL libraries.
 *
 * DQMSHCAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQMSHCAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQMSHCAL.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "AnalysisTools.h"

// -- dqm4hep headers
#include "dqm4hep/DQMPlugin.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMXmlHelper.h"

// -- lcio headers
#include "UTIL/CellIDDecoder.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"

namespace dqm_sdhcal
{

DQM_PLUGIN_DECL( SDHCALEventClassifier , "SDHCALEventClassifier" )
DQM_PLUGIN_DECL( EventHelper , "SDHCALEventHelper" )


EventHelper::EventHelper()
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

EventHelper::~EventHelper()
{
	/* nop */
}

dqm4hep::StatusCode EventHelper::readSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
 m_amplitudeBitRotation = 3;
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                          "AmplitudeBitRotation", m_amplitudeBitRotation));
	m_shiftBCID = 16777216ULL;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "BCIDShift", m_shiftBCID));

  m_newSpillTimeCut = 10;
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "SpillLength", m_newSpillTimeCut));

	m_DAQ_BC_Period = 200; // Size of a clock frame, in nsecond
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "ClockFrameLength", m_DAQ_BC_Period));
  m_DAQ_BC_Period *= 1E-9; // Size of a clock frame, in second

  			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "\n\n\tm_DAQ_BC_Period : " << m_DAQ_BC_Period);

	return dqm4hep::STATUS_CODE_SUCCESS;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


EventClassifier::EventClassifier() :
	m_eventType(UNDEFINED_EVENT),
	m_confidenceLevel(0)
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

EventClassifier::~EventClassifier()
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

bool EventClassifier::isNoisyEvent() const
{
	return ( m_eventType >= SQUARE_NOISE_EVENT && m_eventType <= GROUNDING_NOISE_EVENT );
}

//-------------------------------------------------------------------------------------------------

bool EventClassifier::isPhysicsEvent() const
{
	return ( m_eventType >= BEAM_MUON_EVENT && m_eventType <= CHARGED_HAD_SHOWER_EVENT );
}

//-------------------------------------------------------------------------------------------------

bool EventClassifier::isMuonEvent() const
{
	return ( m_eventType >= BEAM_MUON_EVENT && m_eventType <= COSMIC_MUON_EVENT );
}

//-------------------------------------------------------------------------------------------------

bool EventClassifier::isShowerEvent() const
{
	return ( m_eventType >= SHOWER_EVENT && m_eventType <= CHARGED_HAD_SHOWER_EVENT );
}

//-------------------------------------------------------------------------------------------------

bool EventClassifier::isUndefined() const
{
	return ( m_eventType == UNDEFINED_EVENT );
}

//-------------------------------------------------------------------------------------------------

EventClassifier::EventType EventClassifier::getEventType() const
{
	return m_eventType;
}

//-------------------------------------------------------------------------------------------------

unsigned int EventClassifier::getConfidenceLevel() const
{
	return m_confidenceLevel;
}

//-------------------------------------------------------------------------------------------------

void EventClassifier::setEventType(EventClassifier::EventType type, unsigned int confidenceLevel)
{
	m_eventType = type;
	m_confidenceLevel = confidenceLevel;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

SDHCALEventClassifier::SDHCALEventClassifier() :
	EventClassifier()
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode SDHCALEventClassifier::processEvent(EVENT::LCEvent *pLCEvent)
{
	this->setEventType( EventClassifier::UNDEFINED_EVENT , 100 );

	try
	{
		EVENT::LCCollection *pLCCollection = pLCEvent->getCollection( m_inputCollectionName );
		UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIDDecoder(m_cellIDDecoderString);

		unsigned int nHits = pLCCollection->getNumberOfElements();

		if ( 0 == nHits )
			return dqm4hep::STATUS_CODE_SUCCESS;

		std::set<int> touchedLayers;

		for (unsigned int e = 0 ; e < pLCCollection->getNumberOfElements() ; e++)
		{
			EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit *>(pLCCollection->getElementAt(e));

			if ( NULL == pCaloHit )
				continue;

			int layer = cellIDDecoder(pCaloHit)["K-1"];
			touchedLayers.insert( layer );
		}

		if ( nHits < m_noiseMinNHit )
		{
			this->setEventType( EventClassifier::GROUNDING_NOISE_EVENT , 100 );
			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found noise event (Min N Hit) !" );
			return dqm4hep::STATUS_CODE_SUCCESS;
		}

		if ( touchedLayers.size() < m_noiseMinTouchedLayers )
		{
			this->setEventType( EventClassifier::GROUNDING_NOISE_EVENT , 100 );
			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found noise event (N touched layers) !" );
			return dqm4hep::STATUS_CODE_SUCCESS;
		}

		// content management
		CaloHitMap      caloHitMap;
		CaloHitList     hits;
		CaloClusterList clusters;
		CaloTrackList   tracks;

		CLHEP::Hep3Vector globalHitShift(0, 0, 0);

		try
		{
			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Creating wrapper hits");

			// loop over hits in this event
			for (unsigned int h = 0 ; h < pLCCollection->getNumberOfElements() ; h++)
			{
				EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pLCCollection->getElementAt(h));

				if (NULL == pCaloHit)
					continue;

				int cellID[3];
				cellID[0] = cellIDDecoder(pCaloHit)["I"];
				cellID[1] = cellIDDecoder(pCaloHit)["J"];
				cellID[2] = cellIDDecoder(pCaloHit)["K-1"];

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

			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Creating intra layer clusters");

			for (CaloHitMap::iterator iter = caloHitMap.begin(), endIter = caloHitMap.end() ;
			     iter != endIter ; ++iter)
				m_clusteringAlgorithm.Run(iter->second, clusters);

			std::sort(clusters.begin(), clusters.end(), algorithm::ClusteringHelper::SortClusterByLayer);

			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Filter non isolated clusters");

			CaloClusterList trackingClusters;

			for (CaloClusterList::iterator iter = clusters.begin(), endIter = clusters.end() ;
			     endIter != iter ; ++iter)
				if ( ! m_clusteringHelper.IsIsolatedCluster(*iter, clusters) )
					trackingClusters.push_back(*iter);

			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Run tracking algorithm");

			caloobject::CaloTrack *pTrack = NULL;
			m_trackingAlgorithm.Run(clusters, pTrack);

			if ( NULL != pTrack )
			{
				tracks.push_back(pTrack);

				LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Run interaction finder");

				bool isInteraction = m_interactionFinderAlgorithm.Run(clusters, pTrack->getTrackParameters());
				const float cosThetaTrack(pTrack->getCosTheta());

				if ( ! isInteraction )
				{
					if ( cosThetaTrack < m_muonMaxCosmicCosTheta )
					{
						LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found cosmic muon !" );
						this->setEventType( EventClassifier::COSMIC_MUON_EVENT , 100 );
					}
					else
					{
						LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found beam muon !" );
						this->setEventType( EventClassifier::BEAM_MUON_EVENT , 100 );
					}
				}
			}

			// At this point we found no noise and no muon.
			// Remains EM and HAD showers to analyze
			int startingLayer = this->getStartingLayer(caloHitMap);
			float transverseRatio = m_trackingAlgorithm.getTransverseRatio();
			bool isNeutralHadron = this->isNeutralHadron(caloHitMap);

			// At this point, it remains EM showers and HAD shower cases
			// Apply global cuts first, then cuts to tag EM or HAD shower
			// If doesn't pass global cuts, then flag the event as undefined
			if ( startingLayer >= 0
			     && transverseRatio > m_showerMinTransverseRatio
			     && nHits / static_cast<float>(touchedLayers.size()) > m_showerMinNHitOverNTouchedLayers )
			{
				// flag neutral hadrons
				if ( isNeutralHadron )
				{
					LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found neutral hadron !" );
					this->setEventType( EventClassifier::NEUTRAL_HAD_SHOWER_EVENT, 100 );
					throw dqm4hep::StatusCodeException(dqm4hep::STATUS_CODE_SUCCESS);
				}

				// em shower ?
				if ( touchedLayers.size() < m_electronMaxNTouchedLayers
				     && startingLayer < m_electronMaxStartingLayer )
				{
					LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found EM shower !" );
					this->setEventType( EventClassifier::CHARGED_EM_SHOWER_EVENT, 100 );
					throw dqm4hep::StatusCodeException(dqm4hep::STATUS_CODE_SUCCESS);
				}
				else
				{
					LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found charged hadron !" );
					this->setEventType( EventClassifier::CHARGED_HAD_SHOWER_EVENT, 100 );
					throw dqm4hep::StatusCodeException(dqm4hep::STATUS_CODE_SUCCESS);
				}
			}

			LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Found nothing ! Setting event type to undefined !" );
			throw dqm4hep::StatusCodeException(dqm4hep::STATUS_CODE_SUCCESS);
		}
		catch (dqm4hep::StatusCodeException &exception)
		{
			this->clearEventContents(hits, clusters, tracks);

			if (dqm4hep::STATUS_CODE_SUCCESS != exception.getStatusCode() )
			{
				LOG4CXX_ERROR( dqm4hep::dqmMainLogger ,  "Caught StatusCodeException : " << exception.toString() << ". Skipping event ..." );
				return exception.getStatusCode();
			}
		}
		catch (...)
		{
			this->clearEventContents(hits, clusters, tracks);
			LOG4CXX_ERROR( dqm4hep::dqmMainLogger ,  "Caught unknown exception !" );
			return dqm4hep::STATUS_CODE_FAILURE;
		}
	}
	catch (EVENT::DataNotAvailableException &exception)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Collection " << m_inputCollectionName << " is not available !" );
		return dqm4hep::STATUS_CODE_NOT_FOUND;
	}
	catch (dqm4hep::StatusCodeException &exception)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "SDHCALEventClassifier::processEvent: Caught status code exception : " << exception.toString() );
		return exception.getStatusCode();
	}

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

int SDHCALEventClassifier::getStartingLayer(const SDHCALEventClassifier::CaloHitMap &caloHitMap)
{
	float cogX(0.f), cogY(0.f), cogZ(0.f);
	float sumWeight(0.f);

	for (auto layerIter = caloHitMap.begin(), endLayerIter = caloHitMap.end() ;
	     endLayerIter != layerIter ; ++layerIter)
	{
		for (auto iter = layerIter->second.begin(), endIter = layerIter->second.end() ;
		     endIter != iter ; ++iter)
		{
			float weight(1.f);

			cogX += (*iter)->getCellID()[0] * weight;
			cogY += (*iter)->getCellID()[1] * weight;
			cogZ += (*iter)->getCellID()[2] * weight;

			sumWeight += weight;
		}
	}

	cogX /= sumWeight;
	cogY /= sumWeight;
	cogZ /= sumWeight;

	for (auto layerIter = caloHitMap.begin(), endLayerIter = caloHitMap.end() ;
	     endLayerIter != layerIter ; ++layerIter)
	{
		for (auto iter = layerIter->second.begin(), endIter = layerIter->second.end() ;
		     endIter != iter ; ++iter)
		{
			if ( fabs((*iter)->getCellID()[0] - cogX) > m_startingLayerCogRegion
			     || fabs((*iter)->getCellID()[1] - cogY) > m_startingLayerCogRegion )
				continue;

			std::vector<unsigned int> counters(m_startingLayerNNextLayers, 0);

			for (auto layerIter2 = layerIter, endLayerIter2 = caloHitMap.end() ;
			     endLayerIter2 != layerIter2 ; ++layerIter2)
			{
				if ( layerIter2->first == layerIter->first )
					continue;

				unsigned int layerDifference = layerIter2->first - layerIter->first;

				// look in the 3 next layers for a starting point
				if ( layerDifference > m_startingLayerNNextLayers )
					break;

				for (auto iter2 = layerIter2->second.begin(), endIter2 = layerIter2->second.end() ;
				     endIter2 != iter2 ; ++iter2)
				{
					if ( fabs((*iter2)->getCellID()[0] - cogX) > m_startingLayerCogRegion
					     || fabs((*iter2)->getCellID()[1] - cogY) > m_startingLayerCogRegion )
						continue;

					counters[layerDifference - 1]++;
				}
			}

			if ( counters[0] >= m_startingLayerCounter
			     && counters[1] >= m_startingLayerCounter
			     && counters[2] >= m_startingLayerCounter )
				return static_cast<int>( layerIter->first );
		}
	}

	return -1;
}

//-------------------------------------------------------------------------------------------------

bool SDHCALEventClassifier::isNeutralHadron(const CaloHitMap &caloHitMap)
{
	for (auto iter = caloHitMap.begin(), endIter = caloHitMap.end() ;
	     endIter != iter ; ++iter)
	{
		if ( iter->first >= m_neutralNFirstLayers )
			break;

		if ( iter->second.size() > m_neutralMaxNHitPerLayer )
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------

void SDHCALEventClassifier::clearEventContents(SDHCALEventClassifier::CaloHitList &hits,
    SDHCALEventClassifier::CaloClusterList &clusters, SDHCALEventClassifier::CaloTrackList &tracks)
{
	for_each(hits.begin(), hits.end(), [] (caloobject::CaloHit * pCaloHit) { delete pCaloHit; });
	for_each(clusters.begin(), clusters.end(), [] (caloobject::CaloCluster * pCluster) { delete pCluster; });
	for_each(tracks.begin(), tracks.end(), [] (caloobject::CaloTrack * pTrack) { delete pTrack; });

	hits.clear();
	clusters.clear();
	tracks.clear();
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode SDHCALEventClassifier::readSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
	m_inputCollectionName = "SDHCAL_HIT";
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "InputCollectionName", m_inputCollectionName));

	m_cellIDDecoderString = "I:7,J:7,K-1:6,Dif_id:8,Asic_id:8,Chan_id:6";
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "CellIDDecoderString", m_cellIDDecoderString));

	m_nActiveLayers = 48;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "NActiveLayers", m_nActiveLayers));

	/*----- Noise cut settings -----*/
	m_noiseMinTouchedLayers = 6;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "NoiseMinTouchedLayers", m_noiseMinTouchedLayers));

	m_noiseMinNHit = 10;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "NoiseMinNHit", m_noiseMinNHit));

	/*----- Tracking cut settings -----*/
	m_muonMaxCosmicCosTheta = 0.8;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "MuonMaxCosmicCosTheta", m_muonMaxCosmicCosTheta));

	/*----- Shower cut settings -----*/
	m_neutralNFirstLayers = 5;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "NeutralNFirstLayers", m_neutralNFirstLayers));

	m_neutralMaxNHitPerLayer = 0;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "NeutralMaxNHitPerLayer", m_neutralMaxNHitPerLayer));

	m_startingLayerCogRegion = 5;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "StartingLayerCogRegion", m_startingLayerCogRegion));

	m_startingLayerNNextLayers = 3;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "StartingLayerNNextLayers", m_startingLayerNNextLayers));

	m_startingLayerCounter = 4;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "StartingLayerCounter", m_startingLayerCounter));

	m_showerMinTransverseRatio = 0.01;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ShowerMinTransverseRatio", m_showerMinTransverseRatio));

	m_showerMinNHitOverNTouchedLayers = 3;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ShowerMinNHitOverNTouchedLayers", m_showerMinNHitOverNTouchedLayers));

	m_electronMaxNTouchedLayers = 30;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ElectronMaxNTouchedLayers", m_electronMaxNTouchedLayers));

	m_electronMaxStartingLayer = 5;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ElectronMaxStartingLayer", m_electronMaxStartingLayer));


	/*----- Clustering settings -----*/
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

	m_clusteringAlgorithm.SetClusterParameterSetting(m_clusteringSettings);

	/*----- Clustering helper settings -----*/
	m_clusteringHelperSettings.longitudinalDistance = 100.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ClusteringHelper.LongitudinalDistanceForIsolation", m_clusteringHelperSettings.longitudinalDistance));

	m_clusteringHelperSettings.transversalDistance = 200.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "ClusteringHelper.TransverseDistanceForIsolation", m_clusteringHelperSettings.transversalDistance));

	m_clusteringHelper.SetClusteringHelperParameterSetting(m_clusteringHelperSettings);

	/*----- Tracking settings -----*/
	m_trackingSettings.chiSquareLimit = 100.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Tracking.ChiSquareLimit", m_trackingSettings.chiSquareLimit));

	m_trackingSettings.maxTransverseRatio = 0.05f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Tracking.MaxTransverseRatio", m_trackingSettings.maxTransverseRatio));

	m_trackingSettings.cosThetaLimit = 0.1f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Tracking.CosThetaLimit", m_trackingSettings.cosThetaLimit));

	m_trackingSettings.printDebug = false;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "Tracking.PrintDebug", m_trackingSettings.printDebug));

	m_trackingAlgorithm.SetTrackingParameterSetting(m_trackingSettings);

	/*----- Interaction finder settings -----*/
	m_interactionFinderSettings.minSize = 4;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "InteractionFinder.MinSize", m_interactionFinderSettings.minSize));

	m_interactionFinderSettings.maxRadius = 50.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "InteractionFinder.MaxRadius", m_interactionFinderSettings.maxRadius));

	m_interactionFinderSettings.maxDepth = 4;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "InteractionFinder.MaxDepth", m_interactionFinderSettings.maxDepth));

	m_interactionFinderSettings.minNumberOfCluster = 3;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
	                        "InteractionFinder.MinNumberOfCluster", m_interactionFinderSettings.minNumberOfCluster));

	m_interactionFinderAlgorithm.SetInteractionFinderParameterSetting(m_interactionFinderSettings);

	return dqm4hep::STATUS_CODE_SUCCESS;
}

}

