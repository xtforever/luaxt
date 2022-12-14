#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>



#include <xtcw/mls.h>

#include "luaxt.h"
#include "lua-var5.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <X11/Xaw/AllWidgets.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xft/Xft.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xmu/Editres.h>
#include <X11/Shell.h>


#include "var5.h"
#include "xtcw/wcreg2.h"
#include <xtcw/register_wb.h>
#include <WcCreate.h>
#include <Xp.h>
#include <xtcw/xutil.h>

#define APP_NAME "luaxt"
Widget TopLevel;
XtAppContext LUAXT_APP;

extern int luaopen_luaxt(lua_State* L); 


char *fallback_resources[] = {
    APP_NAME ".allowShellResize: False",
    "*WclResFiles:" APP_NAME ".ad\n",
    "*traceLevel: 2",
    NULL };
static XrmOptionDescRec options[] = {
    { "-TraceLevel",    "*traceLevel",  XrmoptionSepArg, NULL },
    WCL_XRM_OPTIONS
};
typedef struct LUAXT_CONFIG {
    int traceLevel;
} LUAXT_CONFIG ; 
#define FLD(n)  XtOffsetOf(LUAXT_CONFIG,n)
static XtResource LUAXT_CONFIG_RES [] = {

  { "traceLevel", "TraceLevel", XtRInt, sizeof(int),
   FLD(traceLevel), XtRImmediate, 0
  },

  /*
    { NULL, NULL, XtRWidget, sizeof(Widget),
    FLD(widget_grid), XtRString, "*gb"
  },

  { NULL, NULL, XtRWidget, sizeof(Widget),
    FLD(widget_grid), XtRString, "*gb"
  },
  */
  
};
#undef FLD

struct LUAXT_CONFIG LUAXT = { 0 };
void quit_cb( Widget w, void *u, void *c )
{
    TRACE(1,"QUIT");
    XtAppSetExitFlag( XtWidgetToApplicationContext(w) );
}

static void
LUA(Widget w, XtPointer client_data, XtPointer call_data)
{
    if( ! client_data ) {
	WARN("Called without argument");
	return;
    }

    /* can we expand vars inside the lua code? e.g. player=$radio.selection1 */
    luaxt_pushcallback( client_data );
}

static void RegisterApplication ( Widget top )
{
    /* -- Register widget classes and constructors */
    /* RCP( top, weditMV ); */
    /* -- Register application specific actions */
    /* -- Register application specific callbacks */
    RCB( top, quit_cb );
    RCB( top, LUA );
}

static void InitializeApplication( Widget top )
{
    trace_level = LUAXT.traceLevel;
}

static void DestroyApplication(void)
{
    // rc_free();
}

static void syntax(void)
{
  puts( syntax_wcl );
  puts( "-TraceLevel <num>\n"
        "-ListenPort <num>\n" );
}








void proc_exit()
{

                int wstat;
                pid_t   pid;

                while (TRUE) {
                        pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
                        if (pid == 0)
                                return;
                        else if (pid == -1)
                                return;
                        else
                            printf ("Return code: %d\n",WEXITSTATUS( wstat ));
                }
}


static int lua_addargs (lua_State *L) {
    int n = lua_gettop(L);    /* number of arguments */
    lua_Number sum = 0.0;
    int i;
    for (i = 1; i <= n; i++) {
	if (!lua_isnumber(L, i)) {
	    lua_pushliteral(L, "incorrect argument");
	    lua_error(L);
	}
	sum += lua_tonumber(L, i);
    }
    lua_pushnumber(L, sum/n);        /* first result */
    lua_pushnumber(L, sum);         /* second result */
    return 2;                   /* number of results */
}

static int lua_xargs(lua_State *L)
{
    
    luaL_checktype(L,1,LUA_TTABLE);
    int n = lua_rawlen(L,1);   

    for(int i=1; i<=n; i++ ) {
	lua_rawgeti(L,1,i); // push table [i] on stack
	const char* str = luaL_checkstring(L,-1); // table[i]
	puts(str);
	lua_pop(L,1);
    }
    return 0;			     
}

int lua_cfappend(lua_State *L)
{
    // lua_getfield(L, LUA_GLOBALSINDEX, "luaxt" );
    // lua_pushcclosure(L,f,0);
    // lua_setfield(L, LUA_GLOBALSINDEX, "function_name" );

    lua_getglobal(L, "luaxt"); 
    if( ! lua_istable(L,-1) ) {
	puts("table luaxt not found");
	return 0;
    }		
    lua_pushcclosure(L,lua_xargs,0);
    lua_setfield( L, -2, "xargs" ); // stack[-2] == global-table, stack[-1] == c-function-pointer == value, 'xargs' == key
    return 0;
}



int
main(int argc, char **argv)
{
    int status, result, i;
    double sum;
    lua_State *L;
    signal (SIGCHLD, proc_exit);
    m_init();
    mvar_init();
    luaxt_init();
    Widget box2;
    setlocale(LC_ALL, "" );
    XtSetLanguageProc (NULL, NULL, NULL);
    XawInitializeWidgetSet();

    
    TopLevel = XtOpenApplication(&LUAXT_APP, "luaxt",
                                 options, XtNumber(options),
                                 &argc,argv,
                                 fallback_resources,
                                 sessionShellWidgetClass,
                                 NULL, 0 );
    XtAddEventHandler(TopLevel, (EventMask) 0, True, _XEditResCheckMessages, NULL);
    RegisterApplication ( TopLevel  );
    XpRegisterAll ( LUAXT_APP );
    WcWidgetCreation ( TopLevel );
    XtGetApplicationResources(  TopLevel, (XtPointer)&LUAXT,
                                LUAXT_CONFIG_RES,
                                XtNumber(LUAXT_CONFIG_RES),
                                (ArgList)0, 0 );
    InitializeApplication(TopLevel);
    XtRealizeWidget(TopLevel);
    grab_window_quit(TopLevel);
    
    //   
    

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */
    luaopen_luaxt(L); /* load luaxt table i.e. functions generated by swig */
    luaopen_var5( L );
    
    lua_register( L, "addargs", lua_addargs );
    lua_register( L, "xargs",   lua_xargs );
    lua_register( L, "cfappend",   lua_cfappend );


    
    /* Load the file containing the script we are going to run */

    char *fn=argv[1];
    if( argc < 2  ) fn="ex.lua"; 
    status = luaL_loadfile(L, fn );
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /*
     * Ok, now here we go: We pass data to the lua script on the stack.
     * That is, we first have to prepare Lua's virtual stack the way we
     * want the script to receive it, then ask Lua to run it.
     */
    lua_newtable(L);    /* We will pass a table */

    /*
     * To put values into the table, we first push the index, then the
     * value, and then call lua_rawset() with the index of the table in the
     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
     * the top of the stack. When you create the table with lua_newtable(),
     * the table gets pushed into the top of the stack. When you push the
     * index and then the cell value, the stack looks like:
     *
     * <- [stack bottom] -- table, index, value [top]
     *
     * So the -1 will refer to the cell value, thus -3 is used to refer to
     * the table itself. Note that lua_rawset() pops the two last elements
     * of the stack, so that after it has been called, the table is at the
     * top of the stack.
     */
    for (i = 1; i <= 5; i++) {
        lua_pushnumber(L, i);   /* Push the table index */
        lua_pushnumber(L, i*2); /* Push the cell value */
        lua_rawset(L, -3);      /* Stores the pair in the table */
    }

    /* By what name is the script going to reference our table? */
    lua_setglobal(L, "foo");


    /* Ask Lua to run our little script */
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */
    sum = lua_tonumber(L, -1);

    printf("Script returned: %.0f\n", sum);

    lua_pop(L, 1);  /* Take the returned value out of the stack */
    lua_close(L);   /* Cya, Lua */

    XtDestroyWidget(TopLevel);
    DestroyApplication();
    luaxt_destroy();
    m_destruct();
    return EXIT_SUCCESS;
}
