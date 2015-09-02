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


#include "dqmsdhcal/modules/AsicAnalysisModule.h"
#include "dqm4hep/core/DQMMonitorElement.h"
#include "dqm4hep/core/DQMEvent.h"
#include "dqm4hep/module/DQMModuleApi.h"

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

using namespace dqm4hep;

namespace dqm_sdhcal
{

// plugin declaration
AsicAnalysisModule anAsicAnalysisModule;

AsicAnalysisModule::AsicAnalysisModule()
 : DQMAnalysisModule("AsicAnalysisModule")
{
	m_pStreamout = new Streamout();
	m_pTrivent = new Trivent();
}

//-------------------------------------------------------------------------------------------------

AsicAnalysisModule::~AsicAnalysisModule() 
{
	delete 	m_pStreamout;
	delete 	m_pTrivent;
}
//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::initModule()
{
	m_pStreamout->setInputCollectionName(m_streamoutInputCollectionName);
	m_pStreamout->setOutputCollectionName(m_streamoutOutputCollectionName);
	m_pStreamout->setXDaqShift(m_xdaqShift);

	// initialize trivent
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrivent->init());

	// global element used for booking
	// direct element access is provided by dqm4hep module API
	dqm4hep::DQMMonitorElement *pMonitorElement = NULL;

    for(unsigned int l=0 ; l<m_nActiveLayers ; l++)
    {
    	std::stringstream layerDir;
    	layerDir << "/Layer_" << l;

    	DQMModuleApi::mkdir(this, layerDir.str());
    	DQMModuleApi::cd(this, layerDir.str());

    	//------------------------

    	pMonitorElement = NULL;
    	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram2D(this,
    			pMonitorElement, "Efficiency", "Asic efficiency of layer" + DQM4HEP::typeToString(l),
    			12, 0, 12, 12, 0, 12));

    	pMonitorElement->setDescription("Map of asic efficiency of layer " + DQM4HEP::typeToString(l)
    	+ ". The efficiency is evaluated by analysing muons from beam (or cosmics) with a tracking algorithm.\n"
    			"Algorithm author : Arnaud Steen");
    	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);
    	pMonitorElement->setDrawOption("colz");

    	//------------------------

    	pMonitorElement = NULL;
    	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram2D(this,
    			pMonitorElement, "Multiplicity", "Asic multiplicity of layer" + DQM4HEP::typeToString(l),
    			12, 0, 12, 12, 0, 12));

    	pMonitorElement->setDescription("Map of asic multiplicity of layer " + DQM4HEP::typeToString(l)
    	+ ". The multiplicity is evaluated by analysing muons from beam (or cosmics) with a tracking algorithm.\n"
    			"Algorithm author : Arnaud Steen");
    	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);
    	pMonitorElement->setDrawOption("colz");
    }

    DQMModuleApi::mkdir(this, "/Global");
    DQMModuleApi::cd(this, "/Global");

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram1D(this,
			pMonitorElement, "Efficiency", "Global efficiency",
			100, 0, 1));

	pMonitorElement->setDescription(std::string("Global efficiency over the set of asics.")
	+ " The global efficiency is evaluated by analysing muons from beam (or cosmics) with a tracking algorithm.\n"
			+ "  Algorithm author : Arnaud Steen");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);

	//----------------------------------------

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram1D(this,
			pMonitorElement, "Multiplicity", "Global multiplicity",
			160, 0, 16));

	pMonitorElement->setDescription(std::string("Global multiplicity over the set of asics.")
	+ " The global multiplicity is evaluated by analysing muons from beam (or cosmics) with a tracking algorithm.\n"
			"  Algorithm author : Arnaud Steen");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);

	//----------------------------------------

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram2D(this,
			pMonitorElement, "StackedEfficiency", "Efficiency (all asics)",
			12, 0, 12, 12, 0, 12));

	pMonitorElement->setDescription(std::string("Stacked efficiency over the set of asics.")
	+ " The stacked efficiency is evaluated by analysing muons from beam (or cosmics) with a tracking algorithm.\n"
			"  Algorithm author : Arnaud Steen");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);
	pMonitorElement->setDrawOption("colz");

	//----------------------------------------

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram2D(this,
			pMonitorElement, "StackedMultiplicity", "Multiplicity (all asics)",
			12, 0, 12, 12, 0, 12));

	pMonitorElement->setDescription(std::string("Stacked multiplicity over the set of asics.")
	+ " The stacked multiplicity is evaluated by analysing muons from beam (or cosmics) with a tracking algorithm.\n"
			"  Algorithm author : Arnaud Steen");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);
	pMonitorElement->setDrawOption("colz");

	//----------------------------------------

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookRealHistogram1D(this,
			pMonitorElement, "NTrackPerAsic", "N tracks per asic",
			m_expectedNTracksPerAsicOverRun+1, 0, m_expectedNTracksPerAsicOverRun));

	pMonitorElement->setDescription(std::string("The number of reconstructed tracks per asics.\n")
			+ "  Algorithm author : Arnaud Steen");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);

	//----------------------------------------

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookInt(this,
			pMonitorElement, "NRecEvents", "N reconstructed events", 0));

	pMonitorElement->setDescription("The number of reconstructed events in the run");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);

	//----------------------------------------

	pMonitorElement = NULL;
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookInt(this,
			pMonitorElement, "NNoisyEvents", "N noisy events", 0));

	pMonitorElement->setDescription("The number of noisy events in the run");
	pMonitorElement->setResetPolicy(END_OF_RUN_RESET_POLICY);

	DQMModuleApi::cd(this);
	DQMModuleApi::ls(this, true);

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::readSettings(const Json::Value &value)
{
	m_shouldProcessStreamout = value.get("ShouldProcessStreamout", m_shouldProcessStreamout).asBool();
	m_shouldProcessTrivent = value.get("ShouldProcessTrivent", m_shouldProcessTrivent).asBool();

	if(m_shouldProcessStreamout)
	{
		const Json::Value &streamoutValue = value["Streamout"];

		m_streamoutInputCollectionName = streamoutValue.get("InputCollectionName", m_streamoutInputCollectionName).asString();
		m_streamoutOutputCollectionName = streamoutValue.get("OutputCollectionName", m_streamoutOutputCollectionName).asString();
		m_xdaqShift = streamoutValue.get("XDaqShift", 24).asUInt();
	}

	if(m_shouldProcessTrivent)
	{
		const Json::Value &triventValue = value["Trivent"];
		// forward parsing to Trivent
		RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrivent->readSettings(value));
	}

	m_nActiveLayers = value.get("NActiveLayers", 48).asUInt();
	m_expectedNTracksPerAsicOverRun = value.get("ExpectedNTracksPerAsicOverRun", 10000).asUInt();

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::processEvent(DQMEvent *pEvent)
{
	if(NULL == pEvent)
		return STATUS_CODE_FAILURE;

	EVENT::LCEvent *pLCEvent = pEvent->getEvent<EVENT::LCEvent>();

	if(NULL == pLCEvent)
		return STATUS_CODE_FAILURE;

	try
	{
		// process Streamout if needed
		if(m_shouldProcessStreamout)
			THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pStreamout->processEvent(pLCEvent));

		// process Trivent if needed
		if(m_shouldProcessTrivent)
			THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrivent->processEvent(pLCEvent));

		// analyze reconstructed events by Trivent
		const std::vector<EVENT::LCEvent*> &reconstructedEvents(m_pTrivent->getReconstructedEvents());
		unsigned int nRecEvts = reconstructedEvents.size();

		TScalarInt *pNRecScalar = DQMModuleApi::getMonitorElement(this, "/Global", "NRecEvents")->get<TScalarInt>();
		pNRecScalar->Set(pNRecScalar->Get() + nRecEvts);

		const std::vector<EVENT::LCEvent*> &noiseEvents(m_pTrivent->getNoiseEvents());
		unsigned int nNoiseEvts = noiseEvents.size();

		TScalarInt *pNNoiseScalar = DQMModuleApi::getMonitorElement(this, "/Global", "NNoisyEvents")->get<TScalarInt>();
		pNNoiseScalar->Set(pNNoiseScalar->Get() + nNoiseEvts);

		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->doTrackStudy(reconstructedEvents));
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "Caught StatusCodeException : " << exception.toString() << std::endl;
		this->clearContents();
		return exception.getStatusCode();
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
		streamlog_out(ERROR) << "Skipping event" << std::endl;
		this->clearContents();
		return STATUS_CODE_SUCCESS;
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::doTrackStudy(const std::vector<EVENT::LCEvent*> &eventList)
{
	try
	{
		// loop over reconstructed events
		for(std::vector<EVENT::LCEvent*>::const_iterator evtIter = eventList.begin(), evtEndIter = eventList.end() ;
				evtEndIter != evtIter ; ++evtIter)
		{
			EVENT::LCEvent *pLCEvent = *evtIter;

			if(NULL == pLCEvent)
				continue;

			try
			{
				EVENT::LCCollection *pCalorimeterHitCollection = pLCEvent->getCollection(m_triventOutputCollectionName);

				if(NULL == pCalorimeterHitCollection)
					continue;

				UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIDDecoder(pCalorimeterHitCollection);

				// loop over hits in this event
				for(unsigned int h=0 ; h<pCalorimeterHitCollection->getNumberOfElements() ; h++)
				{
					EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pCalorimeterHitCollection->getElementAt(h));

					if(NULL == pCaloHit)
						continue;

					m_calorimeterHitCollection.push_back(pCaloHit);
				}

				this->doTrackStudy();
				m_calorimeterHitCollection.clear();
			}
			catch(EVENT::DataNotAvailableException &exception)
			{
				streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
				streamlog_out(ERROR) << "Skipping event" << std::endl;
				this->clearContents();
				return STATUS_CODE_SUCCESS;
			}
		} // for loop
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "Caught StatusCodeException : " << exception.toString() << std::endl;
		this->clearContents();
		return exception.getStatusCode();
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::doTrackStudy()
{
  UTIL::CellIDDecoder<EVENT::CalorimeterHit> IDdecoder("M:3,S-1:3,I:9,J:9,K-1:6");
  clusters.clear();

  std::vector<EVENT::CalorimeterHit*> _temp;
  int ID = 0;
  int nclusters = 0;
  Cluster* cluster = NULL;

  for(std::vector<EVENT::CalorimeterHit*>::iterator it=m_calorimeterHitCollection.begin(); it!=m_calorimeterHitCollection.end(); ++it)
  {
	  if(std::find(_temp.begin(),_temp.end(), (*it) )!=_temp.end())
		  continue;

    cluster = new Cluster(IDdecoder(*it)["K-1"]);
    cluster->AddHits(*it);
    nclusters++;
    ID++;
    _temp.push_back(*it);

    cluster->BuildCluster(_temp, m_calorimeterHitCollection, (*it));
    cluster->buildClusterPosition();
    cluster->setClusterID(ID);

    clusters.push_back(cluster);
  }

  for(std::vector<Cluster*>::iterator it=clusters.begin(); it!=clusters.end(); ++it)
    (*it)->IsolatedCluster(clusters);

  for(std::vector<Cluster*>::iterator it=clusters.begin(); it!=clusters.end(); ++it)
  {
    if( (*it)->isIsolated() )
    {
      streamlog_out( DEBUG ) << "cluster at " << (*it)->getClusterPosition().x() << " " << (*it)->getClusterPosition().y() << " " << (*it)->getClusterPosition().z()
			     << " is isolated and rejected" << std::endl;
      delete *it;
      clusters.erase(it);
      it--;
    }
  }

  std::sort(clusters.begin(), clusters.end(), ClusterClassFunction::sortDigitalClusterByLayer);

  if(clusters.size() > 5)
  {
    if(TrackSelection(clusters))
    {
      LayerProperties(clusters);
    }
  }

  for(std::vector<Cluster*>::iterator it=clusters.begin(); it!=clusters.end(); ++it)
  {
    delete *it;
  }
}

//-------------------------------------------------------------------------------------------------

bool AsicAnalysisModule::TrackSelection(std::vector<Cluster*> &clVec)
{
  TrackingAlgo* aTrackingAlgo = new TrackingAlgo();

  aTrackingAlgo->Init(clVec);
  aTrackingAlgo->DoTracking();
  bool success = aTrackingAlgo->TrackFinderSuccess();

  delete aTrackingAlgo;

  return success;
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::LayerProperties(std::vector<Cluster*> &clVec)
{
  int trackBegin= (*clVec.begin())->getLayerID();
  int trackEnd=(*(clVec.end()-1))->getLayerID();

  if(trackBegin == 1)
	  trackBegin = 0;

  if(trackEnd == 46)
	  trackEnd = 47;

  for(int K=trackBegin; K<=trackEnd; K++)
  {
    Layer* aLayer = new Layer(K);
    aLayer->Init(clVec);
    aLayer->ComputeLayerProperties();

    int asicKey = findAsicKey(K,aLayer->getxExpected(),aLayer->getyExpected());

    if(asicKey<0)
    {
    	delete aLayer;
    	continue;
    }

    if(asicMap.find(asicKey)==asicMap.end())
    {
      Asic* asic = new Asic(asicKey);
      asicMap[asicKey] = asic;
    }

    if(aLayer->getLayerTag()==fUnefficientLayer)
      asicMap[asicKey]->Update(0);
    if(aLayer->getLayerTag()==fEfficientLayer)
      asicMap[asicKey]->Update(aLayer->getMultiplicity());

    delete aLayer;
  }
}

//-------------------------------------------------------------------------------------------------

int AsicAnalysisModule::findAsicKey(int layer,float x, float y)
{
  float I=round( x/10.408 );
  float J=round( y/10.408 );
  if(I>96||I<0||J>96||J<0) return -1;
  int jnum=(J-1)/8;
  int inum=(I-1)/8;
  int num=jnum*12+inum;
  return layer*1000+num;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::startOfCycle()
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::endOfCycle()
{
	DQMMonitorElement *pMonitorElement = NULL;

	for(int i=0; i<m_nActiveLayers; i++)
	{
		std::stringstream layerDir;
		layerDir << "/Layer_" << i;

		for(std::map<int,Asic*>::iterator it = asicMap.begin(); it != asicMap.end(); it++)
		{
			if(it->first/1000==i)
			{
				if(it->second->getAsicCounter()>0)
				{
					RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, layerDir.str(), "Efficiency", pMonitorElement));
					pMonitorElement->get<TH2F>()->Fill(it->second->getAsicPosition()[0],it->second->getAsicPosition()[1],it->second->getAsicEfficiency()*1.0/it->second->getAsicCounter());

					RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, "/Global", "Efficiency", pMonitorElement));
					pMonitorElement->get<TH1F>()->Fill(it->second->getAsicEfficiency()*1.0/it->second->getAsicCounter());

					RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, "/Global", "StackedEfficiency", pMonitorElement));
					pMonitorElement->get<TH2F>()->Fill(it->second->getAsicPosition()[0],it->second->getAsicPosition()[1],it->second->getAsicEfficiency()*1.0/it->second->getAsicCounter()/(float)m_nActiveLayers);
				}
				if(it->second->getAsicEfficiency()>0)
				{
					RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, layerDir.str(), "Multiplicity", pMonitorElement));
					pMonitorElement->get<TH2F>()->Fill(it->second->getAsicPosition()[0],it->second->getAsicPosition()[1],it->second->getAsicMultiplicity()*1.0/it->second->getAsicEfficiency());

					RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, "/Global", "Multiplicity", pMonitorElement));
					pMonitorElement->get<TH1F>()->Fill(it->second->getAsicMultiplicity()*1.0/it->second->getAsicEfficiency());

					RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, "/Global", "StackedMultiplicity", pMonitorElement));
					pMonitorElement->get<TH2F>()->Fill(it->second->getAsicPosition()[0],it->second->getAsicPosition()[1],it->second->getAsicMultiplicity()*1.0/it->second->getAsicEfficiency()/(float)m_nActiveLayers);
				}

				RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::getMonitorElement(this, "/Global", "NTrackPerAsic", pMonitorElement));
				pMonitorElement->get<TH1F>()->Fill(it->second->getAsicCounter());
			}
		}
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::startOfRun(DQMRun *pRun)
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::endOfRun(DQMRun *pRun)
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode AsicAnalysisModule::endModule()
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::DQMPlugin *AsicAnalysisModule::clone() const
{
	return new AsicAnalysisModule();
}

//-------------------------------------------------------------------------------------------------

void AsicAnalysisModule::clearContents()
{
	m_calorimeterHitCollection.clear();

	for(std::vector<Cluster*>::iterator iter = clusters.begin(), endIter = clusters.end() ;
			endIter != iter ; ++iter)
	{
		delete *iter;
	}

	clusters.clear();

	for(std::map<int,Asic*>::iterator iter = asicMap.begin(), endIter = asicMap.end() ;
			endIter != iter ; ++iter)
	{
		delete iter->second;
	}

	asicMap.clear();
}

}

