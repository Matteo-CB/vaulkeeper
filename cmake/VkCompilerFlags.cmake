function(vk_apply_compiler_flags target)
    target_compile_features(${target} PUBLIC cxx_std_23)

    target_compile_definitions(${target} PRIVATE
        UNICODE
        _UNICODE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        VC_EXTRALEAN
        _CRT_SECURE_NO_WARNINGS
        WINVER=${VK_WINDOWS_MIN_VERSION}
        _WIN32_WINNT=${VK_WINDOWS_MIN_VERSION})

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
            /Zc:__cplusplus
            /Zc:preprocessor
            /Zc:inline
            /Zc:throwingNew
            /Zc:referenceBinding
            /Zc:rvalueCast
            /Zc:strictStrings
            /Zc:ternary
            /utf-8
            /EHsc
            /bigobj
            /diagnostics:caret
            /MP
            /wd4702
            /wd4458
            /wd4459
            /wd4267
            /wd4244)

        if(VK_STRICT_WARNINGS)
            target_compile_options(${target} PRIVATE /WX)
        endif()

        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/O2 /Oi /Ot /Ob3 /Gy /Gw /GL /GS->
            $<$<CONFIG:RelWithDebInfo>:/O2 /Oi /Ob2 /Gy /Gw /Zi>
            $<$<CONFIG:Debug>:/Od /Zi /RTC1>)

        target_link_options(${target} PRIVATE
            $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF /INCREMENTAL:NO>
            $<$<CONFIG:RelWithDebInfo>:/DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO>)

        if(VK_ENABLE_PGO_GENERATE)
            target_compile_options(${target} PRIVATE /GL)
            target_link_options(${target} PRIVATE /LTCG:PGINSTRUMENT)
        endif()
        if(VK_ENABLE_PGO_USE)
            target_compile_options(${target} PRIVATE /GL)
            target_link_options(${target} PRIVATE /LTCG:PGOPTIMIZE)
        endif()
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wshadow -Wconversion -Wsign-conversion
            -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Wunused -Woverloaded-virtual
            -Wnull-dereference -Wdouble-promotion
            -Wformat=2)

        if(VK_STRICT_WARNINGS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()

        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:-O3 -fno-plt>
            $<$<CONFIG:Debug>:-O0 -g3>)
    endif()

    if(VK_ENABLE_ASAN)
        if(MSVC)
            target_compile_options(${target} PRIVATE /fsanitize=address)
        else()
            target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address)
        endif()
    endif()

    if(VK_ENABLE_UBSAN AND NOT MSVC)
        target_compile_options(${target} PRIVATE -fsanitize=undefined)
        target_link_options(${target} PRIVATE -fsanitize=undefined)
    endif()
endfunction()
