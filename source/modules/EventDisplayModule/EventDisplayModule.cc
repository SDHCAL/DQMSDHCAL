/*
 *
 * EventDisplayModule.cc source template automatically generated by a class generator
 * Creation date : jeu. mars 10 2016
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


#include "EventDisplayModule.h"

//-- lcio headers
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <EVENT/LCParameters.h>
#include <UTIL/CellIDDecoder.h>

// -- dqm4hep headers
#include "dqm4hep/DQMMonitorElement.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMModuleApi.h"
#include "dqm4hep/DQMPlugin.h"

namespace dqm_sdhcal
{

DQM_PLUGIN_DECL( EventDisplayModule , "EventDisplayModule" )

//-------------------------------------------------------------------------------------------------

EventDisplayModule::EventDisplayModule() :
		DQMTriventModule()
{

}

//-------------------------------------------------------------------------------------------------

EventDisplayModule::~EventDisplayModule() 
{

}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::userInitModule()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::userReadSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
			"InputCaloHitCollections", m_inputCaloHitCollections, [] (const dqm4hep::StringVector &vec) { return ! vec.empty(); }));

	const unsigned int nCollections = m_inputCaloHitCollections.size();

	m_pEventDisplay3D = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"EventDisplay3D", m_pEventDisplay3D));

	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
			"ColorWeightList", m_colorWeightList, [&] (const dqm4hep::IntVector &vec) { return vec.size() == nCollections; }));

	int markerColor3D = kBlack;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"MarkerColor3D", markerColor3D));

	int markerSize3D = 1;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"MarkerSize3D", markerSize3D));

	int markerStyle3D = 21;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
			"MarkerStyle3D", markerStyle3D));

	m_pEventDisplay3D->get<TH3>()->SetMarkerColor(markerColor3D);
	m_pEventDisplay3D->get<TH3>()->SetMarkerSize(markerSize3D);
	m_pEventDisplay3D->get<TH3>()->SetMarkerStyle(markerStyle3D);

	m_pLastProfileZX = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LastProfileZX", m_pLastProfileZX));

	m_pLastProfileZY = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LastProfileZY", m_pLastProfileZY));

	m_pLastProfileXY = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"LastProfileXY", m_pLastProfileXY));

	m_pCycleStackedProfileZX = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"CycleStackedProfileZX", m_pCycleStackedProfileZX));

	m_pCycleStackedProfileZY = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"CycleStackedProfileZY", m_pCycleStackedProfileZY));

	m_pCycleStackedProfileXY = NULL;
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle,
			"CycleStackedProfileXY", m_pCycleStackedProfileXY));

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::processEvent(EVENT::LCEvent *pLCEvent)
{
	m_pEventDisplay3D->reset();
	m_pLastProfileZX->reset();
	m_pLastProfileZY->reset();
	m_pLastProfileXY->reset();

	LOG4CXX_INFO( dqm4hep::dqmMainLogger , "Processing physics event no " << pLCEvent->getEventNumber() );

	const unsigned int nCollections = m_inputCaloHitCollections.size();

	try
	{
		for(unsigned int c=0 ; c<nCollections ; c++)
		{
			const std::string collectionName(m_inputCaloHitCollections.at(c));
			const int colorWeight(m_colorWeightList.at(c));

			try
			{
				EVENT::LCCollection *pCalorimeterHitCollection = pLCEvent->getCollection(collectionName);
				UTIL::CellIDDecoder<EVENT::CalorimeterHit> decoder(pCalorimeterHitCollection);

				// loop over hits in this event
				for(unsigned int h=0 ; h<pCalorimeterHitCollection->getNumberOfElements() ; h++)
				{
					EVENT::CalorimeterHit *pCaloHit = dynamic_cast<EVENT::CalorimeterHit*>(pCalorimeterHitCollection->getElementAt(h));

					if(NULL == pCaloHit)
						continue;

					const float x(pCaloHit->getPosition()[0]);
					const float y(pCaloHit->getPosition()[1]);
					const float z(pCaloHit->getPosition()[2]);

					m_pEventDisplay3D->get<TH3>()->Fill(z, x, y, colorWeight);

					m_pLastProfileZX->get<TH2>()->Fill(z, x);
					m_pLastProfileZY->get<TH2>()->Fill(z, y);
					m_pLastProfileXY->get<TH2>()->Fill(x, y);

					m_pCycleStackedProfileZX->get<TH2>()->Fill(z, x);
					m_pCycleStackedProfileZY->get<TH2>()->Fill(z, y);
					m_pCycleStackedProfileXY->get<TH2>()->Fill(x, y);
				}

			}
			catch(EVENT::DataNotAvailableException &exception)
			{
				LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "Caught EVENT::DataNotAvailableException : " << exception.what() );
				continue;
			}
		}
	}
	catch(...)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "Caught unknown exception !" );
		return dqm4hep::STATUS_CODE_FAILURE;
	}

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::startOfCycle()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::endOfCycle()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::startOfRun(dqm4hep::DQMRun *const pRun)
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::endOfRun(dqm4hep::DQMRun *const pRun)
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode EventDisplayModule::endModule()
{
	return dqm4hep::STATUS_CODE_SUCCESS;
}

} 

