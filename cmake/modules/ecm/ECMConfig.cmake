
set(ecm_dir "${ThirdPartyDir}/extra-cmake-modules")

set(ECM_FIND_MODULE_DIR "${ecm_dir}/find-modules/")
set(ECM_MODULE_DIR "${ecm_dir}/modules/")
set(ECM_KDE_MODULE_DIR "${ecm_dir}/kde-modules/")
set(ECM_PREFIX "${PACKAGE_PREFIX_DIR}")
set(ECM_MODULE_PATH "${ECM_MODULE_DIR}" "${ECM_FIND_MODULE_DIR}" "${ECM_KDE_MODULE_DIR}")

set(ECM_GLOBAL_FIND_VERSION "${ECM_FIND_VERSION}")

include("${ECM_MODULE_DIR}/ECMUseFindModules.cmake")
