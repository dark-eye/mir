if (MIR_BUILD_PLATFORM_GBM_KMS)
  add_subdirectory(gbm-kms/)
endif()

if (MIR_BUILD_PLATFORM_EGLSTREAM_KMS)
  add_subdirectory(eglstream-kms)
endif()

if (MIR_BUILD_PLATFORM_X11)
  add_subdirectory(x11)
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
