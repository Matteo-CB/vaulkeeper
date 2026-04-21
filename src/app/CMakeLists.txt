set(VK_APP_SOURCES
    main.cpp
    ui/viewmodels/application_view_model.hpp
    ui/viewmodels/application_view_model.cpp
    ui/viewmodels/scan_view_model.hpp
    ui/viewmodels/scan_view_model.cpp
    ui/viewmodels/tree_node_model.hpp
    ui/viewmodels/tree_node_model.cpp
    ui/viewmodels/duplicates_view_model.hpp
    ui/viewmodels/duplicates_view_model.cpp
    ui/viewmodels/uninstall_view_model.hpp
    ui/viewmodels/uninstall_view_model.cpp
    ui/viewmodels/rules_view_model.hpp
    ui/viewmodels/rules_view_model.cpp
    ui/viewmodels/quarantine_view_model.hpp
    ui/viewmodels/quarantine_view_model.cpp
    ui/viewmodels/health_view_model.hpp
    ui/viewmodels/health_view_model.cpp
    ui/theme/theme.hpp
    ui/theme/theme.cpp
    ui/command_palette.hpp
    ui/command_palette.cpp)

qt_add_executable(vaulkeeper WIN32 ${VK_APP_SOURCES})
add_executable(vk::vaulkeeper ALIAS vaulkeeper)

qt_add_qml_module(vaulkeeper
    URI Vaulkeeper
    VERSION 1.0
    QML_FILES
        ui/qml/Main.qml
        ui/qml/HealthPage.qml
        ui/qml/ScanPage.qml
        ui/qml/DuplicatesPage.qml
        ui/qml/UninstallPage.qml
        ui/qml/RulesPage.qml
        ui/qml/QuarantinePage.qml
        ui/qml/HealthReportPage.qml
        ui/qml/components/Sidebar.qml
        ui/qml/components/TopBar.qml
        ui/qml/components/CommandPalette.qml
        ui/qml/components/PrimaryButton.qml
        ui/qml/components/Card.qml
        ui/qml/components/VolumeCard.qml
        ui/qml/components/ScanProgressBar.qml
        ui/qml/components/Treemap.qml
        ui/qml/components/DataTable.qml
        ui/qml/components/Sunburst.qml
        ui/qml/components/Heatmap.qml
        ui/qml/components/SnapshotTimeline.qml
        ui/qml/components/LanguagePicker.qml
    RESOURCES
        ui/resources/icons.qrc
        ui/resources/fonts.qrc)

target_link_libraries(vaulkeeper PRIVATE
    vk::vk_core
    vk::vk_platform_win
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Svg
    Qt6::Concurrent
    Qt6::Network
    spdlog::spdlog
    fmt::fmt
    mimalloc)

target_include_directories(vaulkeeper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
vk_apply_compiler_flags(vaulkeeper)

set_target_properties(vaulkeeper PROPERTIES
    FOLDER "Vaulkeeper"
    OUTPUT_NAME "vaulkeeper")

qt_add_translations(vaulkeeper
    TS_FILES
        ui/resources/i18n/vaulkeeper_en.ts
        ui/resources/i18n/vaulkeeper_fr.ts
        ui/resources/i18n/vaulkeeper_es.ts
        ui/resources/i18n/vaulkeeper_de.ts
        ui/resources/i18n/vaulkeeper_it.ts)
