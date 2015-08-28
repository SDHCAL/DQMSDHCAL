  /// \file TestRawDataModule.cc
/*
 *
 * TestRawDataModule.cc source template automatically generated by a class generator
 * Creation date : mar. ao�t 4 2015
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
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

// -- dqm sdhcal headers
#include "dqmsdhcal/modules/TestRawDataModule.h"
#include "dqmsdhcal/streamout/Streamout.h"
#include "dqmsdhcal/trivent/Trivent.h"

// -- dqm4hep headers
#include "dqm4hep/module/DQMModuleApi.h"
#include "dqm4hep/core/DQMMonitorElement.h"
#include "dqm4hep/core/DQMEvent.h"

// -- lcio headers
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/RawCalorimeterHit.h"
#include "UTIL/CellIDDecoder.h"

using namespace dqm4hep;

namespace dqm_sdhcal
{

TestRawDataModule aTestRawDataModule;

TestRawDataModule::TestRawDataModule() :
		DQMAnalysisModule("TestRawDataModule"),
		m_shouldProcessStreamout(true),
		m_shouldProcessTrivent(true),
		m_streamoutInputCollectionName("RU_XDAQ"),
		m_streamoutOutputCollectionName("DHCALRawHits"),
		m_triventInputCollectionName("DHCALRawHits"),
		m_triventOutputCollectionName("SDHCAL_HIT")
{
	setVersion(1, 0, 0);
	setDetectorName("No detector (raw data test)");

	m_pStreamout = new Streamout();
	m_pTrivent = new Trivent();
}

//-------------------------------------------------------------------------------------------------

TestRawDataModule::~TestRawDataModule() 
{
	delete m_pStreamout;
	delete m_pTrivent;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::initModule()
{
	m_pStreamout->setInputCollectionName(m_streamoutInputCollectionName);
	m_pStreamout->setOutputCollectionName(m_streamoutOutputCollectionName);

	// TODO declare Trivent here and configure it
	// ...

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::mkdir(this, "/RawData"));
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::mkdir(this, "/RawCalorimeterHit"));
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::mkdir(this, "/CalorimeterHit"));
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::mkdir(this, "/CalorimeterHit/Reco"));
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::mkdir(this, "/CalorimeterHit/Noise"));

	// raw data elements
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::cd(this, "/RawData"));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pRawNumberOfElement, "RawNElements", "Number of raw data buffers (LCGOs)", 201, 0., 199.));

	// reconstructed raw calorimeter hits
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::cd(this, "/RawCalorimeterHit"));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pNRawCalorimeterHits, "NRawCalorimeterHits", "Number of raw calorimeter hits", 2000, 0, 1999));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pAmplitudeDistribution, "Amplitude", "Raw calorimeter hit amplitudes", 10, 0, 9));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pTimeDistribution, "Time", "Raw calorimeter hit time", 10000, 0, 10000-1));

	// reconstructed calorimeter hits
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::cd(this, "/CalorimeterHit/Reco"));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pRecNumberOfHits, "RecNHits", "Number of reconstructed hits", 1501, 0, 1500));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pRecICellDistribution, "IRec", "I cell profile", 97, 0, 96));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pRecJCellDistribution, "JRec", "J cell profile", 97, 0, 96));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pRecKCellDistribution, "KRec", "K layer profile", 51, 0, 50));

	// noise calorimeter hits
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::cd(this, "/CalorimeterHit/Noise"));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pNoiseNumberOfHits, "NoiseNHits", "Number of noise hits", 1501, 0, 1500));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pNoiseICellDistribution, "INoise", "I cell profile", 97, 0, 96));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pNoiseJCellDistribution, "JNoise", "J cell profile", 97, 0, 96));

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::bookIntHistogram1D(this,
			m_pNoiseKCellDistribution, "KNoise", "K layer profile", 51, 0, 50));

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::readSettings(const Json::Value &value)
{
	m_shouldProcessStreamout = value.get("ShouldProcessStreamout", m_shouldProcessStreamout).asBool();
	m_shouldProcessTrivent = value.get("ShouldProcessTrivent", m_shouldProcessTrivent).asBool();

	if(m_shouldProcessStreamout)
	{
		m_streamoutInputCollectionName = value.get("StreamoutInputCollectionName", m_streamoutInputCollectionName).asString();
		m_streamoutOutputCollectionName = value.get("StreamoutOutputCollectionName", m_streamoutOutputCollectionName).asString();
	}

	if(m_shouldProcessTrivent)
	{
		m_triventInputCollectionName = value.get("TriventInputCollectionName", m_triventInputCollectionName).asString();
		m_triventOutputCollectionName = value.get("TriventOutputCollectionName", m_triventOutputCollectionName).asString();
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::processEvent(DQMEvent *pEvent)
{
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

		// analyze raw data before and after streamout
		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->analyzeRawData(pLCEvent));

		// analyze reconstructed events by Trivent
		const std::vector<EVENT::LCEvent*> &reconstructedEvents(m_pTrivent->getReconstructedEvents());
		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->analyzeReconstructedEvents(reconstructedEvents));

		// analyze noise events tagged by Trivent
		const std::vector<EVENT::LCEvent*> &noiseEvents(m_pTrivent->getNoiseEvents());
		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->analyzeNoiseEvents(noiseEvents));
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "Caught StatusCodeException : " << exception.toString() << std::endl;
		return exception.getStatusCode();
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
		streamlog_out(ERROR) << "Skipping event" << std::endl;

		return STATUS_CODE_SUCCESS;
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::analyzeRawData(EVENT::LCEvent *pLCEvent)
{
	try
	{
		EVENT::LCCollection *pRawDataCollection = pLCEvent->getCollection(m_streamoutInputCollectionName);
		unsigned int nRawElements = pRawDataCollection->getNumberOfElements();
		m_pRawNumberOfElement->get<TH1I>()->Fill(nRawElements);

		EVENT::LCCollection *pRawHitCollection = pLCEvent->getCollection(m_streamoutOutputCollectionName);
		unsigned int nRawHits = pRawHitCollection->getNumberOfElements();
		m_pNRawCalorimeterHits->get<TH1I>()->Fill(nRawHits);

		// analyze raw hits
		for(unsigned int e=0 ; e<pRawHitCollection->getNumberOfElements() ; e++)
		{
			EVENT::RawCalorimeterHit *pRawCalorimeterHit = dynamic_cast<EVENT::RawCalorimeterHit *>(pRawHitCollection->getElementAt(e));

			if(NULL == pRawCalorimeterHit)
				continue;

			int amplitude = pRawCalorimeterHit->getAmplitude();
			m_pAmplitudeDistribution->get<TH1I>()->Fill(amplitude);

			int time = pRawCalorimeterHit->getTimeStamp();
			m_pTimeDistribution->get<TH1I>()->Fill(time);
		}
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "Caught StatusCodeException : " << exception.toString() << std::endl;
		return exception.getStatusCode();
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
		streamlog_out(ERROR) << "Skipping event" << std::endl;

		return STATUS_CODE_SUCCESS;
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::analyzeReconstructedEvents(const std::vector<EVENT::LCEvent*> &eventList)
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

			EVENT::LCCollection *pCalorimeterHitCollection = pLCEvent->getCollection(m_triventOutputCollectionName);

			if(NULL == pCalorimeterHitCollection)
				continue;

			UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIDDecoder(pCalorimeterHitCollection);

			m_pRecNumberOfHits->get<TH1I>()->Fill(pCalorimeterHitCollection->getNumberOfElements());

			// loop over hits in this event
			for(unsigned int h=0 ; h<pCalorimeterHitCollection->getNumberOfElements() ; h++)
			{
				EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pCalorimeterHitCollection->getElementAt(h));

				if(NULL == pCaloHit)
					continue;

				int I = cellIDDecoder(pCaloHit)["I"];
				int J = cellIDDecoder(pCaloHit)["J"];
				int K = cellIDDecoder(pCaloHit)["K-1"];

				m_pRecICellDistribution->get<TH1I>()->Fill(I);
				m_pRecJCellDistribution->get<TH1I>()->Fill(J);
				m_pRecKCellDistribution->get<TH1I>()->Fill(K);
			}
		}
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "Caught StatusCodeException : " << exception.toString() << std::endl;
		return exception.getStatusCode();
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
		streamlog_out(ERROR) << "Skipping event" << std::endl;

		return STATUS_CODE_SUCCESS;
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::analyzeNoiseEvents(const std::vector<EVENT::LCEvent*> &eventList)
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

			EVENT::LCCollection *pCalorimeterHitCollection = pLCEvent->getCollection(m_triventOutputCollectionName);

			if(NULL == pCalorimeterHitCollection)
				continue;

			UTIL::CellIDDecoder<EVENT::CalorimeterHit> cellIDDecoder(pCalorimeterHitCollection);

			m_pNoiseNumberOfHits->get<TH1I>()->Fill(pCalorimeterHitCollection->getNumberOfElements());

			// loop over hits in this event
			for(unsigned int h=0 ; h<pCalorimeterHitCollection->getNumberOfElements() ; h++)
			{
				EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pCalorimeterHitCollection->getElementAt(h));

				if(NULL == pCaloHit)
					continue;

				int I = cellIDDecoder(pCaloHit)["I"];
				int J = cellIDDecoder(pCaloHit)["J"];
				int K = cellIDDecoder(pCaloHit)["K-1"];

				m_pNoiseICellDistribution->get<TH1I>()->Fill(I);
				m_pNoiseJCellDistribution->get<TH1I>()->Fill(J);
				m_pNoiseKCellDistribution->get<TH1I>()->Fill(K);
			}
		}
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "Caught StatusCodeException : " << exception.toString() << std::endl;
		return exception.getStatusCode();
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		streamlog_out(ERROR) << "Caught EVENT::DataNotAvailableException : " << exception.what() << std::endl;
		streamlog_out(ERROR) << "Skipping event" << std::endl;

		return STATUS_CODE_SUCCESS;
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::startOfCycle()
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::endOfCycle()
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::startOfRun(DQMRun *pRun)
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::endOfRun(DQMRun *pRun)
{
	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode TestRawDataModule::endModule()
{
	return STATUS_CODE_SUCCESS;
}

dqm4hep::DQMPlugin *TestRawDataModule::clone() const
{
	return new TestRawDataModule();
}

} 

