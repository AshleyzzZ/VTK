PROJECT(DICOMParser)

set(vtkDICOMParser_LIBRARIES vtkDICOMParser)
vtk_module_export_info()

INCLUDE_DIRECTORIES(${DICOMParser_SOURCE_DIR})
INCLUDE_DIRECTORIES(${DICOMParser_BINARY_DIR})

IF(WIN32)
  IF(BUILD_SHARED_LIBS)
    SET(DICOM_DLL 1)
  ELSE()
    SET(DICOM_STATIC 1)
  ENDIF()
ENDIF()

SET(DICOM_ANSI_STDLIB 1)

CONFIGURE_FILE(${DICOMParser_SOURCE_DIR}/DICOMCMakeConfig.h.in
               ${DICOMParser_BINARY_DIR}/DICOMCMakeConfig.h)

VTK_ADD_LIBRARY(vtkDICOMParser DICOMFile.cxx DICOMParser.cxx DICOMAppHelper.cxx)

IF(NOT VTK_INSTALL_NO_DEVELOPMENT)
  FILE(GLOB __install_files.h "${DICOMParser_SOURCE_DIR}/*.h")
  FILE(GLOB __install_files.txx "${DICOMParser_SOURCE_DIR}/*.h")
  INSTALL(FILES
    ${__install_files.h} ${__install_files.txx}
    ${DICOMParser_BINARY_DIR}/DICOMCMakeConfig.h
    DESTINATION ${VTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)
ENDIF()
