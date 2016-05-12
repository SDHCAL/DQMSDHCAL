/// \file RawAnalysisModule.cc
/*
 *
 * RawAnalysisModule.cc source template automatically generated by a class generator
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

#include "RawAnalysisModule.h"
#include "Streamout.h"

// -- dqmsdhcal headers
#include "ElectronicsMapping.h"

// -- dqm4hep headers
#include "dqm4hep/DQMMonitorElement.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMModuleApi.h"
#include "dqm4hep/DQMPluginManager.h"
#include "dqm4hep/DQMLogging.h"


// -- std headers
#include <iostream>
#include <fstream>
#include <math.h>

//-- lcio headers
#include <EVENT/LCCollection.h>
// #include <EVENT/CalorimeterHit.h>
#include <IMPL/RawCalorimeterHitImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <UTIL/LCTOOLS.h>

using namespace dqm4hep;

namespace dqm_sdhcal
{

RawCaloHitObject::RawCaloHitObject(dqm4hep::DQMCartesianVector vec, int chanId, int asicId, int difId, int layerId, int threshold, int time, dqm4hep::DQMCartesianVector posShift) :
  m_rawHitPosition(0.f, 0.f, 0.f)
{
  m_chanId = chanId;
  m_asicId = asicId;
  m_difId = difId;
  m_layerId = layerId;
  m_rawHitPosition = vec - posShift;
  m_threshold = threshold;
  m_time = time;
}

// plugin declaration
DQM_PLUGIN_DECL( RawAnalysisModule, "RawAnalysisModule" )

RawAnalysisModule::RawAnalysisModule() :
  DQMAnalysisModule(),
  m_cellReferencePosition(0.f, 0.f, 0.f),
  m_cellSize0(10.408f),
  m_cellSize1(10.408f)
{
  m_pStreamout = new Streamout();
}

//-------------------------------------------------------------------------------------------------
RawAnalysisModule::~RawAnalysisModule()
{
  for (std::vector< CaloHitCollectionConverter *>::iterator iter = m_dataConverters.begin(), endIter = m_dataConverters.end() ;
       endIter != iter ; ++iter)
    delete *iter;

  m_dataConverters.clear();
  delete m_pStreamout;

}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::initModule()
{
  m_eventParameters.nTriggerProcessed = 0;
  m_eventParameters.eventIntegratedTime = 0;
  m_eventParameters.spillIntegratedTime = 0;
  m_eventParameters.totalIntegratedTime = 0;
  m_eventParameters.timeLastTrigger = 0;
  m_eventParameters.timeLastSpill = 0;
  m_eventParameters.nTriggerInSpill = 0;
  m_moduleLogStr = "[RawAnalysisModule]";
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::readSettings(const dqm4hep::TiXmlHandle xmlHandle)
{
  /* ------ Streamout Settings ------ */
  m_shouldProcessStreamout = false;
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                          "ShouldProcessStreamout", m_shouldProcessStreamout));

  if (m_shouldProcessStreamout)
  {
    dqm4hep::TiXmlElement *pXmlElement = xmlHandle.FirstChild("Streamout").Element();
    if ( ! pXmlElement )
      return dqm4hep::STATUS_CODE_NOT_FOUND;

    dqm4hep::TiXmlHandle streamoutHandle(pXmlElement);

    std::string inputCollectionName = "RU_XDAQ";
    RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(streamoutHandle,
                            "InputCollectionName", inputCollectionName));

    std::string outputCollectionName = "DHCALRawHits";
    RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(streamoutHandle,
                            "OutputCollectionName", outputCollectionName));

    unsigned int xdaqShift = 24;
    RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(streamoutHandle,
                            "XDaqShift", xdaqShift));

    m_pStreamout->setInputCollectionName(inputCollectionName);
    m_pStreamout->setOutputCollectionName(outputCollectionName);
    m_pStreamout->setXDaqShift(xdaqShift);
  }
  /* ------ End Streamout Settings ------ */

  /* ------ Detector Settings ------ */
  m_nActiveLayers = 48;
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "NActiveLayers", m_nActiveLayers));

  m_nStartLayerShift = 0;
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                          "NStartLayerShift", m_nStartLayerShift));

  m_nAsicPerDif = 48;
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "NAsicPerDif", m_nAsicPerDif));

  m_nChanPerAsic = 64;
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "NChanPerAsic", m_nChanPerAsic));
  /* ------ End Detector Settings ------ */


  /*------------- Converter settings ------------*/
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
  "RawCollectionNames", m_rawCollectionNames, [] (const dqm4hep::StringVector & vec) { return ! vec.empty(); }));

  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValues(xmlHandle,
  "RecCollectionNames", m_recCollectionNames, [&] (const dqm4hep::StringVector & vec) { return vec.size() == m_rawCollectionNames.size(); }));

  dqm4hep::TiXmlElement *pConvXmlElement = xmlHandle.FirstChild("RawDataConverters").Element();

  if ( ! pConvXmlElement )
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Couldn't find xml element RawDataConverters !" );
    return dqm4hep::STATUS_CODE_NOT_FOUND;
  }
  dqm4hep::TiXmlHandle convertersHandle(pConvXmlElement);

  for (dqm4hep::TiXmlElement *pXmlElement = convertersHandle.FirstChild("converter").Element(); NULL != pXmlElement;
       pXmlElement = pXmlElement->NextSiblingElement("converter"))
  {
    if ( m_dataConverters.size() == m_rawCollectionNames.size() )
    {
      LOG4CXX_WARN( dqm4hep::dqmMainLogger, m_moduleLogStr << " - Warning additional raw data converter plugin loading will be skipped !" );
      break;
    }

    std::string plugin;
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::getAttribute(pXmlElement, "plugin", plugin));

    CaloHitCollectionConverter *pConverter = dqm4hep::DQMPluginManager::instance()->createPluginClass<CaloHitCollectionConverter>(plugin);

    if ( NULL == pConverter )
    {
      LOG4CXX_WARN( dqm4hep::dqmMainLogger, m_moduleLogStr << " - pConverter is NULL!" );
      return dqm4hep::STATUS_CODE_NOT_FOUND;
    }

    dqm4hep::TiXmlHandle converterHandle(pXmlElement);
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, pConverter->readSettings(converterHandle));

    m_dataConverters.push_back( pConverter );
  }

  if ( m_dataConverters.size() != m_rawCollectionNames.size() )
    return dqm4hep::STATUS_CODE_INVALID_PARAMETER;
  /*---------------------------------------------------*/


  /*------------- Read electronics Mapping settings ------------*/
  // Needed to access dif/asic/chan informations
  // TODO Make a vec of electronicsMapping for different collections
  TiXmlElement *pElecMapElement = xmlHandle.FirstChild("electronicsMapping").Element();

  if ( ! pElecMapElement )
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << "Couldn't find xml element electronicsMapping !" );
    return dqm4hep::STATUS_CODE_NOT_FOUND;
  }

  std::string plugin;
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::getAttribute(pElecMapElement, "plugin", plugin));

  m_pElectronicsMapping = dqm4hep::DQMPluginManager::instance()->createPluginClass<dqm4hep::DQMElectronicsMapping>(plugin);

  if ( ! m_pElectronicsMapping )
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << "Couldn't find electronicsMapping plugin called : " << plugin );
    return dqm4hep::STATUS_CODE_NOT_FOUND;
  }

  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, m_pElectronicsMapping->readSettings(dqm4hep::TiXmlHandle(pElecMapElement)));
  /*------------- End Electronics Mapping settings ------------*/

  /*------------- Read Geometry Mapping settings ------------*/
  // Needed here only for Geometry dependent Histogram booking (Know dif/layer association)
  // No need for a DB access here as the geometry was already read from the DB/xml in the electronicsMapping settings
  // Then written to a xml file
  // Just need to ensure the same files are used in the xml settings !!!
  bool readFromDB = false;
  dqm4hep::TiXmlHandle electronicsXmlHandle = dqm4hep::TiXmlHandle(pElecMapElement);

  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                          "ReadFromDB", readFromDB));

  std::string geometryFileName = "geometry_SPS_08_2012.xml";
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                   "GeometryFileName", geometryFileName));

  if ( readFromDB )
  {
    std::string host, user, password, database, beamTest;

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                     "Host", host));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                     "User", user));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                     "Password", password));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                     "Database", database));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                     "BeamTest", beamTest));

    GeometryDBInterface interface;
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, interface.connect( host , user , password , database ));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, interface.dumpGeometry( geometryFileName , beamTest ));
  }

  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                          "CellReferencePosition", m_cellReferencePosition));
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                          "CellSize0", m_cellSize0));
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(electronicsXmlHandle,
                          "CellSize1", m_cellSize1));
  dqm4hep::UIntVector layerMask;
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValues(electronicsXmlHandle,
                          "LayerMask", layerMask));
  dqm4hep::UIntVector difMask;
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValues(electronicsXmlHandle,
                          "DifMask", difMask));

  GeometryXmlIO reader;
  RETURN_RESULT_IF( dqm4hep::STATUS_CODE_SUCCESS, !=, reader.load( geometryFileName , m_geometry, layerMask , difMask ) );

  for (Geometry::const_iterator iter = m_geometry.begin(), endIter = m_geometry.end() ;
       endIter != iter ; ++iter)
    for (DifMapping::const_iterator difIter = iter->second.m_difList.begin(), difEndIter = iter->second.m_difList.end() ;
         difEndIter != difIter ; ++difIter)
      m_difMapping[ difIter->first ] = difIter->second;
  /*------------- End Geometry Mapping settings ------------*/


  /*------ Event Helper settings ------*/
  dqm4hep::TiXmlElement *pEventHelperElement = xmlHandle.FirstChild("eventHelper").Element();

  if ( ! pEventHelperElement )
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "Couldn't find xml element eventHelper !" );
    return dqm4hep::STATUS_CODE_NOT_FOUND;
  }

  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::getAttribute(pEventHelperElement, "plugin", plugin));

  m_pEventHelper = dqm4hep::DQMPluginManager::instance()->createPluginClass<EventHelper>(plugin);

  if ( ! m_pEventHelper )
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "Couldn't find eventHelper plugin called : " << plugin );
    return dqm4hep::STATUS_CODE_NOT_FOUND;
  }

  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, m_pEventHelper->readSettings(dqm4hep::TiXmlHandle(pEventHelperElement)));
  /*------ End Event Helper settings ------*/


  /* ------ Booking Monitor Elements ------ */
  // ------ General ME ------
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicOccupancyAll", m_pAsicOccupancyAll));
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicOccupancyChamber", m_pAsicOccupancyChamber));
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicOccupancyDIF", m_pAsicOccupancyDIF));
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "HitFrequencyMap", m_pHitFrequencyMap));

  // ------ PerLayer ME ------
  for (DifMapping::iterator difIter = m_difMapping.begin(), difEndIter = m_difMapping.end() ;
       difEndIter != difIter ; ++difIter)
  {
    int difId = difIter->first;
    int layerId = difIter->second.m_layerId + m_nStartLayerShift;
    std::string folderPath = "/Noise/Layer" + std::to_string(layerId);

    if (!dqm4hep::DQMModuleApi::dirExists(this, folderPath))
    {
      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::mkdir(this, folderPath));
      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::cd(this, folderPath));

      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "ChamberHitsMap1", m_layerElementMap[layerId].m_pChamberHitsMap1));
      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "ChamberHitsMap2", m_layerElementMap[layerId].m_pChamberHitsMap2));
      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "ChamberHitsMap3", m_layerElementMap[layerId].m_pChamberHitsMap3));
    }

    // ------ PerDIF ME ------
    folderPath += "/DIF" + std::to_string(difId) + "/";
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::mkdir(this, folderPath));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::cd(this, folderPath));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicOccupancy", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicOccupancy));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicOccupancyNumber", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicOccupancyNumber));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicHits1", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicHits1));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicHits2", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicHits2));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicHits3", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicHits3));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicFrequency1", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicFreq1));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicFrequency2", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicFreq2));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicFrequency3", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicFreq3));

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicEventTime", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicEventTime));
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::bookMonitorElement(this, xmlHandle, "AsicEventTimeZoom", m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicEventTimeZoom));
  }
  /* ------ End Booking Monitor Elements ------ */
  LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Finished booking ME ");


  // Print Monitor Elements tree structure
  bool shouldDumpMEStructure = false;
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "DumpMEStructure", shouldDumpMEStructure));
  if (shouldDumpMEStructure)
  {
    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMModuleApi::cd(this, "/"));
    dqm4hep::DQMModuleApi::ls(this, true);
  }

  /* ------ Analysis Settings ------ */
  m_inputCollectionName = "SDHCAL_HIT";
  RETURN_RESULT_IF_AND_IF(dqm4hep::STATUS_CODE_SUCCESS, dqm4hep::STATUS_CODE_NOT_FOUND, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                          "InputCollectionName", m_inputCollectionName));

  m_skipEvent = 0; //=1 to skip first event of the acquisition
  RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, dqm4hep::DQMXmlHelper::readParameterValue(xmlHandle,
                   "NumberOfEventToSkip", m_skipEvent));

  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::performOutputDataConversion(EVENT::LCEvent *pLCEvent)
{
  const unsigned int nCollections(m_rawCollectionNames.size());

  for (unsigned int c = 0 ; c < nCollections ; c++)
  {
    const std::string rawCollectionName( m_rawCollectionNames.at(c) );
    const std::string recCollectionName( m_recCollectionNames.at(c) );
    CaloHitCollectionConverter *pDataConverter( m_dataConverters.at(c) );

    try
    {
      EVENT::LCCollection *pLCCollection = pLCEvent->getCollection( rawCollectionName );
      IMPL::LCCollectionVec *pRecCollection = new IMPL::LCCollectionVec(EVENT::LCIO::CALORIMETERHIT);

      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, pDataConverter->convert(pLCCollection, pRecCollection));

      pLCEvent->addCollection( pRecCollection , recCollectionName );
    }
    catch (EVENT::DataNotAvailableException &exception)
    {
      LOG4CXX_ERROR( dqm4hep::dqmMainLogger , "Caught EVENT::DataNotAvailableException : " << exception.what() );
      continue;
    }
  }
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::processEvent(dqm4hep::DQMEvent * const pEvent)
{
  EVENT::LCEvent *pLCEvent = pEvent->getEvent<EVENT::LCEvent>();

  if (NULL == pLCEvent)
  {
    LOG4CXX_WARN( dqm4hep::dqmMainLogger , m_moduleLogStr << " - pLCEvent is NULL! ");
    return dqm4hep::STATUS_CODE_FAILURE;
  }

  if (pLCEvent->getEventNumber() <= m_skipEvent)
  {
    LOG4CXX_WARN( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Skipping Trigger Event " << pLCEvent->getEventNumber() << " ... skipEvent parameter: " << m_skipEvent );
    return dqm4hep::STATUS_CODE_SUCCESS;
  }

  LOG4CXX_INFO( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Processing Trigger event no " << pLCEvent->getEventNumber() << " runNumber no " << pLCEvent->getRunNumber() << " ..." );
  LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Time Stamp: " << pLCEvent->getTimeStamp() );

  // process Streamout if needed
  if (m_shouldProcessStreamout)
    THROW_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, m_pStreamout->processEvent(pLCEvent));

  // Convert DHCALRawHits to SDHCAL_HIT
  THROW_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, this->performOutputDataConversion(pLCEvent));

  RawCaloHitObject::RawCaloHitList rawCaloHitList;
  m_hitTimeMin = 99999999.;
  m_hitTimeMax = 0;
  m_asicMap.clear();

  try
  {
    EVENT::LCCollection *pRawCalorimeterHitCollection = pLCEvent->getCollection(m_inputCollectionName);
    LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " Created collection with type : " << pRawCalorimeterHitCollection->getTypeName());
    LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " NumberOfHits in trigger : " << pRawCalorimeterHitCollection->getNumberOfElements() );

    if (NULL == pRawCalorimeterHitCollection)
    {
      LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " NULL Pointer: pCalorimeterHitCollection pointer " );
      return dqm4hep::STATUS_CODE_SUCCESS;
    }

    //Find Triggers and NewSpill
    StatusCode status = m_pEventHelper->findTrigger<EVENT::RawCalorimeterHit>(pRawCalorimeterHitCollection, m_eventParameters);
    if (dqm4hep::STATUS_CODE_SUCCESS != status)
      return dqm4hep::STATUS_CODE_SUCCESS;

    // Loop over hits in this event & Fill the rawCaloHitCollection
    for (unsigned int h = 0 ; h < pRawCalorimeterHitCollection->getNumberOfElements() ; ++h)
    {
      EVENT::RawCalorimeterHit *pRawCaloHit = dynamic_cast<EVENT::RawCalorimeterHit*>(pRawCalorimeterHitCollection->getElementAt(h));
      if (NULL == pRawCaloHit)
        continue;

      int cellID = pRawCaloHit->getCellID0();

      unsigned int hitThresh = m_pEventHelper->getThreshold(pRawCaloHit);
      unsigned int hitTime = pRawCaloHit->getTimeStamp();

      dqm4hep::DQMElectronicsMapping::Electronics electronics;

      // extract raw data electronics ids
      electronics.m_difId = m_pEventHelper->getDifId( cellID );
      electronics.m_asicId = m_pEventHelper->getAsicId( cellID );
      electronics.m_channelId = m_pEventHelper->getChannelId( cellID );

      // perform conversion to cell ids and absolute position
      dqm4hep::DQMCartesianVector position(0.f, 0.f, 0.f);
      dqm4hep::DQMCartesianVector globalHitShift(0.f, 0.f, 0.f);

      dqm4hep::DQMElectronicsMapping::Cell cell;
      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, m_pElectronicsMapping->electronicsToPosition(electronics, position));
      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, m_pElectronicsMapping->electronicstoCell(electronics, cell));

      unsigned int difId = electronics.m_difId;
      unsigned int asicId = electronics.m_asicId;
      unsigned int chanId = electronics.m_channelId;

      if ( cell.m_layer >= m_nActiveLayers )
      {
        LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Wrong number of layer in your configuration file!");
        LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Found a hit in layer " << cell.m_layer << " - Last layer in xml configuration file is " << m_nActiveLayers);
        continue;
      }

      RawCaloHitObject *pRawCaloHitObject = new RawCaloHitObject(
        position,
        chanId,
        asicId,
        difId,
        cell.m_layer,
        hitThresh,
        hitTime,
        globalHitShift);

      // TODO: There is regularly hits with timeStamp 4294967295=858.993s. Not physical && don't know why
      if (hitTime > m_eventParameters.eventIntegratedTime + 1) // +1== Lost of trigger have Physics hits at m_eventParameters.eventIntegratedTime+1
      {
        LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - hitTime > m_eventParameters.eventIntegratedTime!");
        LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Amplitude: " << hitThresh
                       << "\t Layer : " << cell.m_layer
                       << "\t dif : " << difId
                       << "\t asic : " << asicId
                       << "\t chan : " << chanId
                       << "\t I : " << cell.m_iCell
                       << "\t J : " << cell.m_jCell
                       << "\t time : " << hitTime
                       << "\t time (s): " << hitTime * m_DAQ_BC_Period
                       << "\t m_eventParameters.eventIntegratedTime : " << m_eventParameters.eventIntegratedTime
                       << "\t m_eventParameters.eventIntegratedTime (s) : " << m_eventParameters.eventIntegratedTime * m_DAQ_BC_Period
                     );
        continue;
      }

      if (hitTime < m_hitTimeMin) m_hitTimeMin = hitTime;
      if (hitTime > m_hitTimeMax) m_hitTimeMax = hitTime;

      if (hitThresh <= 0 || hitThresh > 3)
      {
        LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << difId << " had:" << asicId << ":" << chanId << ":" << hitTime << ":" << hitThresh);
        continue;
      }
      bool thr[3];

      thr[0] = (hitThresh == 2);  // First Threshold
      thr[1] = (hitThresh == 1);  // Second Threshold
      thr[2] = (hitThresh == 3);  // Third Threshold

      // ---------- Fill /Layer/DIF
      TH1* h_asicHit1 = m_layerElementMap[cell.m_layer].m_difElementMap[difId].m_pAsicHits1->get<TH1>();
      TH1* h_asicHit2 = m_layerElementMap[cell.m_layer].m_difElementMap[difId].m_pAsicHits2->get<TH1>();
      TH1* h_asicHit3 = m_layerElementMap[cell.m_layer].m_difElementMap[difId].m_pAsicHits3->get<TH1>();

      if (thr[0])
      {
        h_asicHit1->SetBinContent((asicId - 1) * m_nChanPerAsic + chanId + 1, h_asicHit1->GetBinContent((asicId - 1) * m_nChanPerAsic + chanId + 1) + 1);
        m_layerElementMap[cell.m_layer].m_pChamberHitsMap1->get<TH2>()->Fill(cell.m_iCell, cell.m_jCell);
      }
      if (thr[1])
      {
        h_asicHit2->SetBinContent((asicId - 1) * m_nChanPerAsic + chanId + 1, h_asicHit2->GetBinContent((asicId - 1) * m_nChanPerAsic + chanId + 1) + 1);
        m_layerElementMap[cell.m_layer].m_pChamberHitsMap2->get<TH2S>()->Fill(cell.m_iCell, cell.m_jCell);
      }
      if (thr[2])
      {
        h_asicHit3->SetBinContent((asicId - 1) * m_nChanPerAsic + chanId + 1, h_asicHit3->GetBinContent((asicId - 1) * m_nChanPerAsic + chanId + 1) + 1);
        m_layerElementMap[cell.m_layer].m_pChamberHitsMap3->get<TH2>()->Fill(cell.m_iCell, cell.m_jCell);
      }

      m_layerElementMap[cell.m_layer].m_difElementMap[difId].m_pAsicEventTime->get<TH1>()->Fill(hitTime);
      m_layerElementMap[cell.m_layer].m_difElementMap[difId].m_pAsicEventTimeZoom->get<TH1>()->Fill(hitTime);

      m_pHitFrequencyMap->get<TH2>()->SetBinContent(difId + 1, asicId, m_pHitFrequencyMap->get<TH2>()->GetBinContent(difId + 1, asicId) + 1);

      RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, fillAsicOccupancyMap(pRawCaloHitObject));
      rawCaloHitList.push_back(pRawCaloHitObject);
    }

    // Fill hit frequency
    for (DifMapping::iterator difIter = m_difMapping.begin(), difEndIter = m_difMapping.end() ;
         difEndIter != difIter ; ++difIter)
    {
      int difId = difIter->first;
      int layerId = difIter->second.m_layerId + m_nStartLayerShift;

      TH1* h_asicHit1 = m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicHits1->get<TH1>();
      TH1* h_asicHit2 = m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicHits2->get<TH1>();
      TH1* h_asicHit3 = m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicHits3->get<TH1>();
      TH1* h_asicFreq1 = m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicFreq1->get<TH1>();
      TH1* h_asicFreq2 = m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicFreq2->get<TH1>();
      TH1* h_asicFreq3 = m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicFreq3->get<TH1>();

      for (int iBin = 0; iBin < h_asicHit1->GetNbinsX(); ++iBin)
      {
        h_asicFreq1->SetBinContent(iBin, h_asicHit1->GetBinContent(iBin) / (m_eventParameters.eventIntegratedTime));
        h_asicFreq2->SetBinContent(iBin, h_asicHit2->GetBinContent(iBin) / (m_eventParameters.eventIntegratedTime));
        h_asicFreq3->SetBinContent(iBin, h_asicHit3->GetBinContent(iBin) / (m_eventParameters.eventIntegratedTime));
      }
    }

    LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " m_hitTimeMin : " << m_hitTimeMin << "\t m_hitTimeMax : " << m_hitTimeMax << "\t eventIntegratedTime (s): " << m_eventParameters.eventIntegratedTime * m_DAQ_BC_Period << "s\t spillIntegratedTime (s): " << m_eventParameters.spillIntegratedTime * m_DAQ_BC_Period << "s\t totalIntegratedTime (s) : " << m_eventParameters.totalIntegratedTime * m_DAQ_BC_Period << "s");

    RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, doAsicStudy());

    // Analyse rawCaloHits
    //
    // RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, doDIFStudy(pCalorimeterHitCollection));
    // RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, doAsicStudy(pCalorimeterHitCollection));
    // RETURN_RESULT_IF(dqm4hep::STATUS_CODE_SUCCESS, !=, doNoiseStudy(pCalorimeterHitCollection));
  }
  catch (EVENT::DataNotAvailableException &exception)
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Caught EVENT::DataNotAvailableException : " << exception.what() );
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " - Skipping event" );
    return dqm4hep::STATUS_CODE_SUCCESS;
  }
  catch (...)
  {
    LOG4CXX_ERROR( dqm4hep::dqmMainLogger , m_moduleLogStr << " Caught unknown exception !");
    return dqm4hep::STATUS_CODE_FAILURE;
  }
  m_eventParameters.nTriggerProcessed++;
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
int RawAnalysisModule::createAsicKey(int layerId, int difId, int asicId)
{
  return (layerId << 16) | (difId << 8) | asicId;
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::fillAsicOccupancyMap( RawCaloHitObject * const pRawCaloHitObject)
{
  int asicKey = createAsicKey(pRawCaloHitObject->getLayerId(), pRawCaloHitObject->getDifId(), pRawCaloHitObject->getAsicId());

  std::map<int, int>::iterator findAsicIter = m_asicMap.find(asicKey);
  if (m_asicMap.find(asicKey) != m_asicMap.end())
    findAsicIter->second = findAsicIter->second + 1;
  else
  {
    int n = 1;
    std::pair<int, int> p(asicKey, n);
    m_asicMap.insert(p);
  }
  return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::doAsicStudy()
{
  for (std::map<int, int>::iterator asicIter = m_asicMap.begin(); asicIter != m_asicMap.end(); asicIter++)
  {
    unsigned int layerId = (asicIter->first >> 16) & 0xFF;
    unsigned int difId = (asicIter->first >> 8) & 0xFF;
    unsigned int asicId = (asicIter->first) & 0xFF;

    TH1F* hAsicOccupancyDIF = m_pAsicOccupancyDIF->get<TH1F>();
    TH1F* hAsicOccupancyChamber = m_pAsicOccupancyChamber->get<TH1F>();

    m_DAQ_BC_Period = m_pEventHelper->getDAQ_BC_Period();
    m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicOccupancyNumber->get<TH1F>()->SetBinContent(asicId, asicIter->second);
    m_layerElementMap[layerId].m_difElementMap[difId].m_pAsicOccupancy->get<TH1F>()->SetBinContent(asicId, asicIter->second / (m_eventParameters.eventIntegratedTime * m_DAQ_BC_Period));

    m_pAsicOccupancyAll->get<TH1F>()->SetBinContent(difId * m_nAsicPerDif + asicId, asicIter->second / (m_eventParameters.eventIntegratedTime * m_DAQ_BC_Period));
    float fOccupancy = asicIter->second / (m_eventParameters.eventIntegratedTime * m_DAQ_BC_Period);

    if (fOccupancy > hAsicOccupancyChamber->GetBinContent(layerId)) hAsicOccupancyChamber->SetBinContent(layerId, fOccupancy);
    if (fOccupancy > hAsicOccupancyDIF->GetBinContent(difId)) hAsicOccupancyDIF->SetBinContent(difId, fOccupancy);

  }
  return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode RawAnalysisModule::startOfCycle()
{
  // this->resetElements();
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode RawAnalysisModule::endOfCycle()
{
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode RawAnalysisModule::startOfRun(dqm4hep::DQMRun * const pRun)
{
  m_eventParameters.totalIntegratedTime = 0;
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

dqm4hep::StatusCode RawAnalysisModule::endOfRun(dqm4hep::DQMRun * const pRun)
{
  LOG4CXX_DEBUG( dqm4hep::dqmMainLogger , m_moduleLogStr << " Run lasted " << m_eventParameters.totalIntegratedTime * m_DAQ_BC_Period << "s : " << floor(m_eventParameters.totalIntegratedTime * m_DAQ_BC_Period / 3600) << "h " << floor(fmod(m_eventParameters.totalIntegratedTime * m_DAQ_BC_Period / 60, 60)) << "min " << fmod(m_eventParameters.totalIntegratedTime * m_DAQ_BC_Period, 60) <<  "s");
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
dqm4hep::StatusCode RawAnalysisModule::endModule()
{
  return dqm4hep::STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
void RawAnalysisModule::resetElements()
{
  for (std::map<unsigned int, LayerElements>::const_iterator iter = m_layerElementMap.begin(), endIter = m_layerElementMap.end() ;
       endIter != iter ; ++iter)
  {
    iter->second.m_pChamberHitsMap1->reset();
    iter->second.m_pChamberHitsMap2->reset();
    iter->second.m_pChamberHitsMap3->reset();

    for (std::map<int, DifElements>::const_iterator difIter = iter->second.m_difElementMap.begin(), difEndIter = iter->second.m_difElementMap.end() ;
         difEndIter != difIter ; ++difIter)
    {
      difIter->second.m_pAsicHits1->reset();
      difIter->second.m_pAsicHits2->reset();
      difIter->second.m_pAsicHits3->reset();

      difIter->second.m_pAsicFreq1->reset();
      difIter->second.m_pAsicFreq2->reset();
      difIter->second.m_pAsicFreq3->reset();

      difIter->second.m_pAsicOccupancy->reset();
      difIter->second.m_pAsicOccupancyNumber->reset();

      difIter->second.m_pAsicEventTime->reset();
      difIter->second.m_pAsicEventTimeZoom->reset();
    }
  }

  m_pAsicOccupancyAll->reset();
  m_pAsicOccupancyChamber->reset();
  m_pAsicOccupancyDIF->reset();
  m_pHitFrequencyMap->reset();
}

}
