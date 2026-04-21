find_program(WIX_CANDLE NAMES candle.exe)
find_program(WIX_LIGHT NAMES light.exe)

if(NOT WIX_CANDLE OR NOT WIX_LIGHT)
    message(WARNING "WiX Toolset not found. Installer target will be skipped.")
    return()
endif()

set(VK_INSTALLER_OUT "${CMAKE_BINARY_DIR}/installer")
file(MAKE_DIRECTORY "${VK_INSTALLER_OUT}")

set(VK_INSTALLER_CULTURES en-us fr-fr es-es de-de it-it)
set(VK_BRAND_DIR "${CMAKE_SOURCE_DIR}/assets/brand")
set(VK_THEME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/theme")
set(VK_LOC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/loc")
set(VK_PRODUCT_VERSION "${CMAKE_PROJECT_VERSION}.0")
set(VK_REDIST_DIR "${CMAKE_SOURCE_DIR}/third_party/vc_redist" CACHE PATH "Folder containing vc_redist.x64.exe")

set(MSI_TARGETS)
foreach(culture ${VK_INSTALLER_CULTURES})
    set(stage "${VK_INSTALLER_OUT}/${culture}")
    set(msi "${stage}/vaulkeeper-${culture}.msi")

    add_custom_command(
        OUTPUT "${msi}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${stage}"
        COMMAND ${WIX_CANDLE} -nologo
            -dProductVersion=${VK_PRODUCT_VERSION}
            -dBrandDir=${VK_BRAND_DIR}
            -dBuildBinDir=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -ext WixUIExtension
            -ext WixUtilExtension
            -out "${stage}/"
            "${CMAKE_CURRENT_SOURCE_DIR}/vaulkeeper.wxs"
        COMMAND ${WIX_LIGHT} -nologo
            -cultures:${culture}
            -loc "${VK_LOC_DIR}/${culture}.wxl"
            -ext WixUIExtension
            -ext WixUtilExtension
            -out "${msi}"
            "${stage}/vaulkeeper.wixobj"
        DEPENDS vaulkeeper vk VaulkeeperAgent vk-updater vaulkeeper_shellext
                "${CMAKE_CURRENT_SOURCE_DIR}/vaulkeeper.wxs"
                "${VK_LOC_DIR}/${culture}.wxl"
        WORKING_DIRECTORY "${stage}"
        COMMENT "Building MSI for ${culture}")

    list(APPEND MSI_TARGETS "${msi}")
endforeach()

add_custom_target(vaulkeeper_msi DEPENDS ${MSI_TARGETS})

set(BUNDLE_PRIMARY "${VK_INSTALLER_OUT}/en-us/vaulkeeper-en-us.msi")
set(BUNDLE_EXE "${VK_INSTALLER_OUT}/VaulkeeperSetup.exe")

add_custom_command(
    OUTPUT "${BUNDLE_EXE}"
    COMMAND ${WIX_CANDLE} -nologo
        -dProductVersion=${VK_PRODUCT_VERSION}
        -dBrandDir=${VK_BRAND_DIR}
        -dThemeDir=${VK_THEME_DIR}
        -dRedistDir=${VK_REDIST_DIR}
        -dMsiFile=${BUNDLE_PRIMARY}
        -ext WixBalExtension
        -ext WixUtilExtension
        -out "${VK_INSTALLER_OUT}/"
        "${CMAKE_CURRENT_SOURCE_DIR}/bundle.wxs"
    COMMAND ${WIX_LIGHT} -nologo
        -ext WixBalExtension
        -ext WixUtilExtension
        -loc "${VK_THEME_DIR}/bundle-theme.wxl"
        -out "${BUNDLE_EXE}"
        "${VK_INSTALLER_OUT}/bundle.wixobj"
    DEPENDS vaulkeeper_msi
            "${CMAKE_CURRENT_SOURCE_DIR}/bundle.wxs"
            "${VK_THEME_DIR}/bundle-theme.xml"
            "${VK_THEME_DIR}/bundle-theme.wxl"
    WORKING_DIRECTORY "${VK_INSTALLER_OUT}"
    COMMENT "Building VaulkeeperSetup.exe")

add_custom_target(vaulkeeper_installer ALL DEPENDS "${BUNDLE_EXE}")
set_target_properties(vaulkeeper_installer PROPERTIES FOLDER "Vaulkeeper")
