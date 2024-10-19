
function(SET_SOURCE_GROUP source_files)

	SET(INDEX 0)
	
	WHILE(INDEX LESS ${ARGC})
	
		SET(source_file ${ARGV${INDEX}})
		STRING(FIND ${source_file} "/" lastidx REVERSE)
		
		STRING(SUBSTRING ${source_file} 0 ${lastidx} dirpath)
		STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" relpath ${dirpath})
		STRING(COMPARE EQUAL "${relpath}" "" empty)
		
		if(NOT ${empty} EQUAL 1)
			STRING(REPLACE "/" "\\" relpath ${relpath})
			STRING(LENGTH ${relpath} groupnamepath)
			STRING(SUBSTRING ${relpath} 1 ${groupnamepath}-1 groupname)
		else()
			SET(groupname "/")
		endif(NOT ${empty} EQUAL 1)
		
		#message(STATUS "----source_file: ${source_file}")
		#message(STATUS "----groupname: ${groupname}")
		SOURCE_GROUP(${groupname} FILES ${source_file})
		math(EXPR INDEX "${INDEX} + 1")
		
	ENDWHILE()
	
endfunction(SET_SOURCE_GROUP)