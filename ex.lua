

function WidgetSetValue( widget, resource, value )

   print( "search widget   "  ..  widget )
   print( "change resource "  ..  resource )
   print( "new value       "  ..  value )
   
end



function test_callback()

	 x=5 y=6
	print( "test callback " .. x .. y )
	luaxt.luaxt_pushcallback( "hello world" )
	str = luaxt.luaxt_pullcallback()
	print( "Rececived: " .. str )

	luaxt.luaxt_pushcallback( "cb1" )
	luaxt.luaxt_pushcallback( "cb2" )
	luaxt.luaxt_pushcallback( "cb3" )

	repeat
		str = luaxt.luaxt_pullcallback()
		if str ~= "" then
	   	   print( "other string waiting" )
		end

	until str == ""

end


print ("- lua has started -")

--- simple lookup
p=var5[ "test" ]

p[0] = 'entry-0'
print( "p val: "  .. p[0] )
print( p )

name=var5[ "test" ][0]
print( 'this is x '.. name )
x=0

repeat

	fin = luaxt.luaxt_processevent()

	repeat
	  str = luaxt.luaxt_pullcallback()
	  if str ~= "" then
	     print("you want me to evaluate this? " .. str )
	     f = load( str )
	     if f then f() end
	  end
        until str==""

until   fin ~= 0

print ("- lua has stopped -")


