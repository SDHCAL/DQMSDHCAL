  /// \file DQMTriventModule.cc
/*
 *
 * DQMTriventModule.cc source template automatically generated by a class generator
 * Creation date : mar. mars 8 2016
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


#include "DQMTriventModule.h"
#include "Trivent.h"

// -- dqm4hep headers
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMPluginManager.h"

// -- lcio headers
#include "EVENT/LCIO.h"
#include "UTIL/LCTOOLS.h"
#include "Exceptions.h"
#include "EVENT/LCEvent.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCCollectionVec.h"

namespace dqm_sdhcal
{

DQMTriventModule::DQMTriventModule() :
		DQMAnalysisModule(),
		LCTriventListener(),
		m_moduleLogStr("[DQMTriventModule]")
{
	m_pTrivent = new trivent::Trivent();
	m_pTrivent->addListener(this);
}

//-------------------------------------------------------------------------------------------------

DQMTriventModule::~DQMTriventModule() 
{
	delete m_pTrivent;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode DQMTriventModule::readSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
	dqm4hep::TiXmlElement *pXmlElement = xmlHandle.FirstChild("Trivent").Element();

	if( ! pXmlElement )
		return dqm4hep::STATUS_CODE_NOT_FOUND;

	dqm4hep::TiXmlHandle triventHandle(pXmlElement);

	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValues(triventHandle,
			"CaloHitCollectionNames", m_caloHitCollectionNames));

	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValues(triventHandle,
			"RawCaloHitCollectionNames", m_rawCaloHitCollectionNames));

	if(m_caloHitCollectionNames.empty() && m_rawCaloHitCollectionNames.empty())
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger, m_moduleLogStr << " - At least one hit collection type mustn't be empty !" );
		return dqm4hep::STATUS_CODE_INVALID_PARAMETER;
	}

	/*---------------------------------------------------*/

	m_triventParameters.m_timeWindow = 2;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(triventHandle,
			"TimeWindow", m_triventParameters.m_timeWindow));

	m_triventParameters.m_minPeakSize = 10;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(triventHandle,
			"MinPeakSize", m_triventParameters.m_minPeakSize));

	m_triventParameters.m_minElements = 10;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(triventHandle,
			"MinElements", m_triventParameters.m_minElements));

	m_triventParameters.m_maxElements = std::numeric_limits<uint32_t>::max();
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(triventHandle,
			"MaxElements", m_triventParameters.m_maxElements));

	dqm4hep::StringVector maskCollectionNames;
	RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValues(triventHandle,
			"MaskCollectionNames", maskCollectionNames));

	m_triventParameters.m_maskCollectionNames.insert(maskCollectionNames.begin(), maskCollectionNames.end());

	// forward parsing to Trivent
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, this->userReadSettings(xmlHandle));

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode DQMTriventModule::initModule()
{
	m_pTrivent->init(m_triventParameters);
	RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, this->userInitModule());

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode DQMTriventModule::processEvent(dqm4hep::DQMEvent *const pEvent)
{
	EVENT::LCEvent *pLCEvent = pEvent->getEvent<EVENT::LCEvent>();

	if(NULL == pLCEvent)
		return dqm4hep::STATUS_CODE_FAILURE;

	LOG4CXX_INFO( dqm4hep::dqmMainLogger, m_moduleLogStr << " - Processing dqm event no " << pLCEvent->getEventNumber() << " with collections: ");
	for (uint32_t iCol =0; iCol< pLCEvent->getCollectionNames()->size(); ++iCol)
		LOG4CXX_INFO( dqm4hep::dqmMainLogger, "\t\t'" << pLCEvent->getCollectionNames()->at( iCol ) << "'" );


	try
	{
		trivent::Event triventEvent;

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger, m_moduleLogStr << " - converting event..." );
		THROW_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, this->convertEvent(pLCEvent, triventEvent));

		LOG4CXX_DEBUG( dqm4hep::dqmMainLogger, m_moduleLogStr << " - processing trivent algorithm" );
		m_pTrivent->processEvent(triventEvent);
	}
	catch(dqm4hep::StatusCodeException &exception)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Caught StatusCodeException : " << exception.toString() );
		return exception.getStatusCode();
	}
	catch(EVENT::DataNotAvailableException &exception)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Caught EVENT::DataNotAvailableException : " << exception.what() );
		return dqm4hep::STATUS_CODE_SUCCESS;
	}
	catch(...)
	{
		LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Caught unknown exception !" );
		return dqm4hep::STATUS_CODE_FAILURE;
	}

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode DQMTriventModule::convertEvent(EVENT::LCEvent *pLCEvent, trivent::Event &event)
{
	for (auto &collection : m_caloHitCollectionNames)
	{
		try
		{
			trivent::LCTrivent::addCollection<EVENT::CalorimeterHit>(pLCEvent,
					collection, event, &EVENT::CalorimeterHit::getTime);
		}
		catch(EVENT::DataNotAvailableException &exception)
		{
			LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Caught EVENT::DataNotAvailableException : " << exception.what() );
			continue;
		}
	}

	for (auto &collection : m_rawCaloHitCollectionNames)
	{
		try
		{
			 trivent::LCTrivent::addCollection<EVENT::RawCalorimeterHit>(pLCEvent,
					 collection, event, &EVENT::RawCalorimeterHit::getTimeStamp);
		}
		catch(EVENT::DataNotAvailableException &exception)
		{
			LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Caught EVENT::DataNotAvailableException : " << exception.what() );
			continue;
		}
	}

	event.setUserEvent( (void *) pLCEvent );

	return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

void DQMTriventModule::processReconstructedEvent(EVENT::LCEvent *pLCEvent)
{
	LOG4CXX_DEBUG( dqm4hep::dqmMainLogger, m_moduleLogStr << " - processReconstructedEvent : event no " << pLCEvent->getEventNumber() );
	THROW_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, this->processEvent(pLCEvent));
	LOG4CXX_DEBUG( dqm4hep::dqmMainLogger, m_moduleLogStr << " - processReconstructedEvent... OK" );
}

}

