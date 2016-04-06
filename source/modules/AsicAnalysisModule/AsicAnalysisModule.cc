  /// \file AsicAnalysisModule.cc
/*
 *
 * AsicAnalysisModule.cc source template automatically generated by a class generator
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


#include "AsicAnalysisModule.h"

// -- dqm4hep headers
#include "dqm4hep/DQMMonitorElement.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMModuleApi.h"
#include "dqm4hep/DQMPlugin.h"

// -- std headers
#include <iostream>
#include <fstream>

//-- lcio headers
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <EVENT/LCParameters.h>
#include <UTIL/CellIDDecoder.h>

#include "streamlog/streamlog.h"

using namespace dqm4hep;

namespace dqm_sdhcal
{

// plugin declaration
DQM_PLUGIN_DECL( AsicAnalysisModule, "AsicAnalysisModule" )

AsicAnalysisModule::AsicAnalysisModule() :
	DQMTriventModule()
{
}

//-------------------------------------------------------------------------------------------------

AsicAnalysisModule::~AsicAnalysisModule() 
{
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::userReadSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
	m_nActiveLayers = 48;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"NActiveLayers", m_nActiveLayers));

	m_nAsicX = 12;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"NAsicX", m_nAsicX));

	m_nAsicY = 12;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"NAsicY", m_nAsicY));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
			"AsicTable", m_asicTable, [&] (const IntVector &vec) { return vec.size() == m_nActiveLayers; }));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
			"DifList", m_difList, [] (const IntVector &vec) { return ! vec.empty(); }));

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
	m_trackingSettings.chiSquareLimit = 100.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Tracking.ChiSquareLimit", m_trackingSettings.chiSquareLimit));

	m_trackingSettings.maxTransverseRatio = 0.05f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Tracking.MaxTransverseRatio", m_trackingSettings.maxTransverseRatio));

	m_trackingSettings.cosThetaLimit = 0.9f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Tracking.CosThetaLimit", m_trackingSettings.cosThetaLimit));

	m_trackingSettings.printDebug = false;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Tracking.PrintDebug", m_trackingSettings.printDebug));

	/*-----------------------------------------------------*/
	m_efficiencySettings.maxRadius = 25.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Efficiency.MaxRadius", m_efficiencySettings.maxRadius));

	m_efficiencySettings.semiDigitalReadout = true;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Efficiency.SDHCALReadout", m_efficiencySettings.semiDigitalReadout));

	m_efficiencySettings.printDebug = false;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Efficiency.PrintDebug", m_efficiencySettings.printDebug));

	/*-----------------------------------------------------*/
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

	/*-----------------------------------------------------*/
	m_layerSettings.edgeX_min = 0.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Layer.EdgeX_Min", m_layerSettings.edgeX_min));

	m_layerSettings.edgeX_max = 1000.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Layer.EdgeX_Max", m_layerSettings.edgeX_max));

	m_layerSettings.edgeY_min = 0.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Layer.EdgeY_Min", m_layerSettings.edgeY_min));

	m_layerSettings.edgeY_max = 1000.f;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"Layer.EdgeY_Max", m_layerSettings.edgeY_max));

	/*-----------------------------------------------------*/
	const StatusCode statusCode = dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
			"AsicKeyFinder.KeyFactor", m_asicKeyFinderSettings.keyFactors);

	if(dqm4hep::STATUS_CODE_NOT_FOUND == statusCode)
		m_asicKeyFinderSettings.keyFactors = { 1, 12, 1000 };
	else if(dqm4hep::STATUS_CODE_SUCCESS != statusCode)
		return statusCode;

	m_asicKeyFinderSettings.nPadX = 96;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"AsicKeyFinder.NPadX", m_asicKeyFinderSettings.nPadX));

	m_asicKeyFinderSettings.nPadY = 96;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"AsicKeyFinder.NPadY", m_asicKeyFinderSettings.nPadY));

	m_asicKeyFinderSettings.asicNPad = 8;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"AsicKeyFinder.AsicNPad", m_asicKeyFinderSettings.asicNPad));

	m_asicKeyFinderSettings.layerGap = 26.131;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"AsicKeyFinder.LayerGap", m_asicKeyFinderSettings.layerGap));

	m_asicKeyFinderSettings.padSize = 10.408;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"AsicKeyFinder.PadSize", m_asicKeyFinderSettings.padSize));

	m_asicKeyFinderSettings.printDebug = false;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"AsicKeyFinder.PrintDebug", m_asicKeyFinderSettings.printDebug));

	/*-----------------------------------------------------*/
	for(unsigned int l=0 ; l<m_nActiveLayers ; l++)
	{
		RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
				"EfficiencyMap", l, m_layerElementMap[l].m_pEfficiencyMap));

		RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
				"Efficiency2Map", l, m_layerElementMap[l].m_pEfficiency2Map));

		RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
				"Efficiency3Map", l, m_layerElementMap[l].m_pEfficiency3Map));

		RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
				"MultiplicityMap", l, m_layerElementMap[l].m_pMultiplicityMap));
	}

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LayerEfficiency", m_pLayerEfficiency));

	m_pLayerEfficiency->get<TGraph>()->SetMarkerSize(1);
	m_pLayerEfficiency->get<TGraph>()->SetMarkerColor(kBlack);
	m_pLayerEfficiency->get<TGraph>()->SetMarkerStyle(23);

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LayerEfficiency2", m_pLayerEfficiency2));

	m_pLayerEfficiency2->get<TGraph>()->SetMarkerSize(1);
	m_pLayerEfficiency2->get<TGraph>()->SetMarkerColor(kBlack);
	m_pLayerEfficiency2->get<TGraph>()->SetMarkerStyle(23);

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LayerEfficiency3", m_pLayerEfficiency3));

	m_pLayerEfficiency3->get<TGraph>()->SetMarkerSize(1);
	m_pLayerEfficiency3->get<TGraph>()->SetMarkerColor(kBlack);
	m_pLayerEfficiency3->get<TGraph>()->SetMarkerStyle(23);

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LayerMultiplicity", m_pLayerMultiplicity));

	m_pLayerMultiplicity->get<TGraph>()->SetMarkerSize(1);
	m_pLayerMultiplicity->get<TGraph>()->SetMarkerColor(kBlack);
	m_pLayerMultiplicity->get<TGraph>()->SetMarkerStyle(23);

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"AsicEfficiency", m_pAsicEfficiency));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"AsicEfficiency2", m_pAsicEfficiency2));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"AsicEfficiency3", m_pAsicEfficiency3));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"AsicMultiplicity", m_pAsicMultiplicity));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"StackedEfficiencyMap", m_pStackedEfficiencyMap));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"StackedEfficiency2Map", m_pStackedEfficiency2Map));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"StackedEfficiency3Map", m_pStackedEfficiency3Map));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"StackedMultiplicityMap", m_pStackedMultiplicityMap));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"GlobalEfficiency", m_pGlobalEfficiency));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"GlobalEfficiency2", m_pGlobalEfficiency2));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"GlobalEfficiency3", m_pGlobalEfficiency3));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"GlobalMultiplicity", m_pGlobalMultiplicity));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"NTracksPerAsic", m_pNTracksPerAsic));

	m_inputCollectionName = "SDHCAL_HIT";
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"InputCollectionName", m_inputCollectionName));

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::userInitModule()
{
	// initialize algorithms
	m_clusteringAlgorithm.SetClusterParameterSetting(m_clusteringSettings);
	m_clusteringHelper.SetClusteringHelperParameterSetting(m_clusteringHelperSettings);
	m_trackingAlgorithm.SetTrackingParameterSetting(m_trackingSettings);
	m_interactionFinderAlgorithm.SetInteractionFinderParameterSetting(m_interactionFinderSettings);
	m_efficiencyAlgorithm.SetEfficiencyParameterSetting(m_efficiencySettings);
	m_asicKeyFinderAlgorithm.SetAsicKeyFinderParameterSetting(m_asicKeyFinderSettings);

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::processNoisyEvent(EVENT::LCEvent *pLCEvent)
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::processPhysicalEvent(EVENT::LCEvent *pLCEvent)
{
	LOG4CXX_INFO( dqm4hep::dqmMainLogger , "Processing physics event no " << pLCEvent->getEventNumber() );

	// content management
	caloobject::CaloHitMap caloHitMap;
	std::vector< caloobject::CaloHit *> hits;
	std::vector< caloobject::CaloCluster *> clusters;
	std::vector< caloobject::CaloTrack *>   tracks;

	CLHEP::Hep3Vector globalHitShift(0, 0, 0);

	try
	{
		EVENT::LCCollection *pCalorimeterHitCollection = pLCEvent->getCollection(m_inputCollectionName);

		if(NULL == pCalorimeterHitCollection)
			return dqm4hep::STATUS_CODE_SUCCESS;

		UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIDDecoder(m_cellIDDecoderString);

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Creating wrapper hits");

		// loop over hits in this event
		for(unsigned int h=0 ; h<pCalorimeterHitCollection->getNumberOfElements() ; h++)
		{
			EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pCalorimeterHitCollection->getElementAt(h));

			if(NULL == pCaloHit)
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

		for(caloobject::CaloHitMap::iterator iter = caloHitMap.begin(), endIter = caloHitMap.end() ;
				iter != endIter ; ++iter)
			m_clusteringAlgorithm.Run(iter->second, clusters);

		std::sort(clusters.begin(), clusters.end(), algorithm::ClusteringHelper::SortClusterByLayer);

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , "Filter non - isolated clusters");

		caloobject::CaloClusterList trackingClusters;

		for(std::vector<caloobject::CaloCluster*>::iterator iter = clusters.begin(), endIter = clusters.end() ;
				endIter != iter ; ++iter)
			if( ! m_clusteringHelper.IsIsolatedCluster(*iter, clusters) )
				trackingClusters.push_back(*iter);

		caloobject::CaloTrack *pTrack = NULL;

		m_trackingAlgorithm.Run(clusters, pTrack);

		// stop processing if no reconstructed track
		if( NULL == pTrack )
			throw dqm4hep::StatusCodeException(dqm4hep::STATUS_CODE_SUCCESS);

		tracks.push_back(pTrack);

		bool isInteraction = m_interactionFinderAlgorithm.Run(clusters, pTrack->getTrackParameters());

		// tracking on muons only
		// stop processing if event is an interaction
		if( isInteraction )
			throw dqm4hep::StatusCodeException(dqm4hep::STATUS_CODE_SUCCESS);

		// update analysis contents : asics and layers

		int trackBegin = (*clusters.begin())->getLayerID();
		int trackEnd = (*(clusters.rbegin()))->getLayerID();

		if(1 == trackBegin)
			trackBegin = 0;

		if(m_nActiveLayers-2 == trackEnd)
			trackEnd = m_nActiveLayers-1;

		for(unsigned int l = trackBegin ; l<=trackEnd ; l++)
		{
			// reset layer properties
			m_caloLayerList.at(l)->Reset();

			// and re-evaluate efficiency of layer
			m_efficiencyAlgorithm.Run(m_caloLayerList.at(l), clusters);

			if( 0 == m_caloLayerList.at(l)->getNTracks() )
				continue;

			int key = m_asicKeyFinderAlgorithm.FindAsicKey( m_efficiencyAlgorithm.getExpectedPosition() );

			caloobject::SDHCALAsicMap::iterator findIter = m_asicMap.find(key);

			if( findIter == m_asicMap.end() )
				continue;

			findIter->second->Update( m_caloLayerList.at(l) );
		}

		this->clearEventContents(hits, clusters, tracks);
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
		streamlog_out(ERROR) << "Skipping event" << std::endl;
		this->clearEventContents(hits, clusters, tracks);
		return STATUS_CODE_SUCCESS;
	}
	catch(...)
	{
		streamlog_out(ERROR) << "Caught unknown exception !" << std::endl;
		this->clearEventContents(hits, clusters, tracks);
		return STATUS_CODE_FAILURE;
	}

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::startOfCycle()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::endOfCycle()
{
	struct LayerInfo
	{
		float           m_efficiency;
		float           m_efficiency2;
		float           m_efficiency3;
		float           m_multiplicity;
		unsigned int    m_count;
		unsigned int    m_efficientCount;
	};

	std::map<unsigned int, LayerInfo> layerInfoMap;

	this->resetElements();

	for(caloobject::SDHCALAsicMap::iterator iter = m_asicMap.begin(), endIter = m_asicMap.end() ;
			endIter != iter ; ++iter)
	{
		if( ! (iter->second->getAsicNtrack() > 0) )
			continue;

		const unsigned int nTracks = iter->second->getAsicNtrack();
		const unsigned int layerID = iter->second->getAsicKey()/1000;

		const float efficiency1 = iter->second->getAsicEfficiency();
		const float efficiency2 = iter->second->getAsicEfficiency2();
		const float efficiency3 = iter->second->getAsicEfficiency3();

		const float x = iter->second->getPosition()[0];
		const float y = iter->second->getPosition()[1];


		const bool isEfficient = efficiency1 > 0.f;

		const float efficiencyError1 = std::sqrt(efficiency1 * (1 - efficiency1) / nTracks);
		const float efficiencyError2 = std::sqrt(efficiency2 * (1 - efficiency2) / nTracks);
		const float efficiencyError3 = std::sqrt(efficiency3 * (1 - efficiency3) / nTracks);

		std::map<unsigned int, LayerInfo>::iterator infoIter = layerInfoMap.find(layerID);

		if( layerInfoMap.end() == infoIter )
		{
			infoIter = layerInfoMap.insert(std::map<unsigned int, LayerInfo>::value_type(layerID, LayerInfo())).first;
			infoIter->second.m_efficiency = 0.f;
			infoIter->second.m_efficiency2 = 0.f;
			infoIter->second.m_efficiency3 = 0.f;
			infoIter->second.m_multiplicity = 0.f;
			infoIter->second.m_count = 0;
			infoIter->second.m_efficientCount = 0;
		}

		infoIter->second.m_efficiency += efficiency1;
		infoIter->second.m_efficiency2 += efficiency2;
		infoIter->second.m_efficiency3 += efficiency3;
		infoIter->second.m_count ++;

		std::map<unsigned int, LayerElements>::iterator layerIter = m_layerElementMap.find(layerID);

		// global fill
		m_pNTracksPerAsic->get<TH1>()->Fill( nTracks );

		// fill efficiency
		m_pAsicEfficiency->get<TH1F>()->Fill( efficiency1 );
		m_pAsicEfficiency2->get<TH1F>()->Fill( efficiency2 );
		m_pAsicEfficiency3->get<TH1F>()->Fill( efficiency3 );
		m_pStackedEfficiencyMap->get<TH2F>()->Fill(x, y, efficiency1 / m_nActiveLayers);

		if( m_layerElementMap.end() != layerIter )
		{
			layerIter->second.m_pEfficiencyMap->get<TH2>()->Fill(x, y, efficiency1);
			layerIter->second.m_pEfficiency2Map->get<TH2>()->Fill(x, y, efficiency2);
			layerIter->second.m_pEfficiency3Map->get<TH2>()->Fill(x, y, efficiency3);
		}

		// fill multiplicity
		if( isEfficient )
		{
			const float multiplicity = iter->second->getAsicMultiplicity();
			m_pAsicMultiplicity->get<TH1F>()->Fill( multiplicity );
			m_pStackedMultiplicityMap->get<TH2F>()->Fill(x, y, multiplicity / m_nActiveLayers);

			if( m_layerElementMap.end() != layerIter )
				layerIter->second.m_pMultiplicityMap->get<TH2>()->Fill(x, y, multiplicity);

			infoIter->second.m_multiplicity += multiplicity;
			infoIter->second.m_efficientCount ++;
		}
	}

	LayerInfo globalInfo;
	globalInfo.m_efficiency = 0.f;
	globalInfo.m_efficiency2 = 0.f;
	globalInfo.m_efficiency3 = 0.f;
	globalInfo.m_multiplicity = 0.f;
	globalInfo.m_count = 0;
	globalInfo.m_efficientCount = 0;

	// fill efficiency and multiplicity per layer
	// compute the global efficiency and multiplicity for all layers
	for(std::map<unsigned int, LayerInfo>::iterator iter = layerInfoMap.begin(), endIter = layerInfoMap.end() ;
			endIter != iter ; ++iter)
	{
		const unsigned int layerID = iter->first;

		if(iter->second.m_count > 0)
		{
			const float layerEfficiency = ( iter->second.m_efficiency / iter->second.m_count );
			const float layerEfficiency2 = ( iter->second.m_efficiency2 / iter->second.m_count );
			const float layerEfficiency3 = ( iter->second.m_efficiency3 / iter->second.m_count );

			Int_t pointID = m_pLayerEfficiency->get<TGraph>()->GetN();
			m_pLayerEfficiency->get<TGraph>()->SetPoint(pointID, layerID , layerEfficiency * 100 );
			m_pLayerEfficiency2->get<TGraph>()->SetPoint(pointID, layerID , layerEfficiency2 * 100 );
			m_pLayerEfficiency3->get<TGraph>()->SetPoint(pointID, layerID , layerEfficiency3 * 100 );

			globalInfo.m_efficiency += layerEfficiency;
			globalInfo.m_efficiency2 += layerEfficiency2;
			globalInfo.m_efficiency3 += layerEfficiency3;
			globalInfo.m_count ++;
		}

		if(iter->second.m_efficientCount > 0)
		{
			const float layerMultiplicity = ( iter->second.m_multiplicity / iter->second.m_efficientCount );

			Int_t pointID = m_pLayerMultiplicity->get<TGraph>()->GetN();
			m_pLayerMultiplicity->get<TGraph>()->SetPoint( pointID , layerID , layerMultiplicity );

			globalInfo.m_multiplicity += layerMultiplicity;
			globalInfo.m_efficientCount ++;
		}
	}

	// set global detector efficiency and multiplicity
	m_pGlobalEfficiency->get< TScalarObject<float> >()->Clear();
	m_pGlobalEfficiency2->get< TScalarObject<float> >()->Clear();
	m_pGlobalEfficiency3->get< TScalarObject<float> >()->Clear();
	m_pGlobalMultiplicity->get< TScalarObject<float> >()->Clear();

	if( globalInfo.m_count > 0 )
	{
		m_pGlobalEfficiency->get< TScalarObject<float> >()->Set( ( globalInfo.m_efficiency / globalInfo.m_count) * 100 );
		m_pGlobalEfficiency2->get< TScalarObject<float> >()->Set( ( globalInfo.m_efficiency2 / globalInfo.m_count) * 100 );
		m_pGlobalEfficiency3->get< TScalarObject<float> >()->Set( ( globalInfo.m_efficiency3 / globalInfo.m_count) * 100 );
	}

	if( globalInfo.m_efficientCount > 0 )
		m_pGlobalMultiplicity->get< TScalarObject<float> >()->Set( globalInfo.m_multiplicity / globalInfo.m_efficientCount );

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::startOfRun(DQMRun *pRun)
{
	this->createAsicAndLayerContents();

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::endOfRun(DQMRun *pRun)
{
	this->clearContents();
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::endModule()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::clearEventContents(caloobject::CaloHitList &hits, caloobject::CaloClusterList &clusters, caloobject::CaloTrackList &tracks)
{
	for_each(hits.begin(), hits.end(), [] (caloobject::CaloHit *pCaloHit) { delete pCaloHit; });
	for_each(clusters.begin(), clusters.end(), [] (caloobject::CaloCluster *pCluster) { delete pCluster; });
	for_each(tracks.begin(), tracks.end(), [] (caloobject::CaloTrack *pTrack) { delete pTrack; });

	hits.clear();
	clusters.clear();
	tracks.clear();
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::createAsicAndLayerContents()
{
	// build layers and asics
	for(unsigned int l=0 ; l<m_nActiveLayers ; ++l)
	{
		caloobject::CaloLayer *pLayer = new caloobject::CaloLayer(l);

		pLayer->setLayerParameterSetting(m_layerSettings);
		m_caloLayerList.push_back(pLayer);

		for(unsigned int ax=0 ; ax<m_nAsicX ; ax++)
		{
			for(unsigned int ay=0 ; ay<m_nAsicY ; ay++)
			{
				int key = m_asicKeyFinderAlgorithm.BuildAsicKey(ax, ay, l);

				caloobject::SDHCAL_Asic *pAsic = new caloobject::SDHCAL_Asic(key);
				pAsic->setASIC_ID( this->findDifID(key) , this->findAsicID(key) );

				m_asicMap.insert(caloobject::SDHCALAsicMap::value_type( key, pAsic ));
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::clearContents()
{
	for(caloobject::CaloLayerList::iterator iter = m_caloLayerList.begin(), endIter = m_caloLayerList.end() ;
			endIter != iter ; ++iter)
		delete *iter;

	for(caloobject::SDHCALAsicMap::iterator iter = m_asicMap.begin(), endIter = m_asicMap.end() ;
			endIter != iter ; ++iter)
		delete iter->second;

	m_caloLayerList.clear();
	m_asicMap.clear();
}

//-------------------------------------------------------------------------------------------------

int AsicAnalysisModule::findDifID(int key)
{
	return m_difList.at( key/1000*3 + 2 - key%1000%12/4 );
}

//-------------------------------------------------------------------------------------------------

int AsicAnalysisModule::findAsicID(int key)
{
	return m_asicTable.at( 4*(key%1000/12) + key%1000%12%4 );
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::resetElements()
{
	for(std::map<unsigned int, LayerElements>::iterator iter = m_layerElementMap.begin(), endIter = m_layerElementMap.end() ;
			endIter != iter ; ++iter)
	{
		iter->second.m_pEfficiencyMap->reset();
		iter->second.m_pEfficiency2Map->reset();
		iter->second.m_pEfficiency3Map->reset();
		iter->second.m_pMultiplicityMap->reset();
	}

	m_pLayerEfficiency->get<TGraph>()->Set(0);
	m_pLayerEfficiency2->get<TGraph>()->Set(0);
	m_pLayerEfficiency3->get<TGraph>()->Set(0);
	m_pLayerMultiplicity->get<TGraph>()->Set(0);

	m_pAsicEfficiency->reset();
	m_pAsicEfficiency2->reset();
	m_pAsicEfficiency3->reset();
	m_pAsicMultiplicity->reset();

	m_pStackedEfficiencyMap->reset();
	m_pStackedEfficiency2Map->reset();
	m_pStackedEfficiency3Map->reset();
	m_pStackedMultiplicityMap->reset();

	m_pGlobalEfficiency->reset();
	m_pGlobalEfficiency2->reset();
	m_pGlobalEfficiency3->reset();
	m_pGlobalMultiplicity->reset();

	m_pNTracksPerAsic->reset();
}

}

