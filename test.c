#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>
#include <gdk/gdkkeysyms.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#ifndef LIBXML_TREE_ENABLED
#error LIBXML_TREE_ENABLED Undefined!
#endif

#ifdef USE_GETTEXT
#include "libintl.h"
#include "locale.h"
#define _(String) gettext (String)
#else
#define _(String) String
#endif

#ifndef GDK_KEY_space
#        define GDK_KEY_space GDK_space
#        define GDK_KEY_Right GDK_Right
#        define GDK_KEY_KP_Add GDK_KP_Add
#        define GDK_KEY_KP_Subtract GDK_KP_Subtract
#        define GDK_KEY_plus GDK_plus
#        define GDK_KEY_minus GDK_minus
#        define GDK_KEY_F7 GDK_F1
#        define GDK_KEY_F8 GDK_F2
#        define GDK_KEY_F6 GDK_F3
#        define GDK_KEY_F7 GDK_F4
#        define GDK_KEY_F8 GDK_F5
#        define GDK_KEY_F6 GDK_F6
#        define GDK_KEY_F7 GDK_F7
#        define GDK_KEY_F8 GDK_F8
#        define GDK_KEY_q GDK_q
/* GDK_KEY_space */
#endif

static void cb_destroy_window(GtkWidget* widget, GtkWidget* window) {
	gtk_main_quit();
}


static gboolean cb_close_web_view(WebKitWebView* webView, GtkWidget* window) {
	gtk_widget_destroy(window);
	return TRUE;
}


static void cb_load_status(WebKitWebView *web_view, GParamSpec * pspec, void * p_context) {
	WebKitLoadStatus status = webkit_web_view_get_load_status (web_view);
	GObject *object = G_OBJECT (web_view);

	g_object_freeze_notify (object);

	switch (status) {
		case WEBKIT_LOAD_FINISHED:
			printf("WEBKIT_LOAD_FINISHED\n");
			// Load Javascript Language Translator
			char load_js_script[] = "load_js(\"blockly/lua_compressed.js\", null); setup_blockly(\"\");";
			webkit_web_view_execute_script(web_view, load_js_script);
			break;

		case WEBKIT_LOAD_PROVISIONAL:
			printf("WEBKIT_LOAD_PROVISIONAL\n");
			break;

		case WEBKIT_LOAD_COMMITTED:
			printf("WEBKIT_LOAD_COMMITTED\n");
			break;

		case WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT:
			printf("WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT\n");
			break;

		case WEBKIT_LOAD_FAILED:
			printf("WEBKIT_LOAD_FAILED\n");
			break;

		default:
			break;
	}

	g_object_thaw_notify (object);
}


static void cb_execute(GtkWidget* widget, void * p_data) {
	WebKitWebView * view = (WebKitWebView *)p_data;
	char script[] = "Sys.print(Blockly.Xml.domToPrettyText(Blockly.Xml.workspaceToDom(Blockly.mainWorkspace)));";
	//char script[] = "Sys.print(Blockly.Lua.workspaceToCode());";
	webkit_web_view_execute_script(view, script);
}


static void cb_sysclass_init(JSContextRef ctx, JSObjectRef object) {
	g_message("Initializing Sys Namespace.");
}


static void cb_sysclass_finalize(JSObjectRef object) {
	g_message("Custom class finalize.");
}

static JSValueRef cb_sysclass_print(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception) {
	/* At least, one argument must be received */
	if (argumentCount == 1 && JSValueIsString(context, arguments[0])) {
		/* Converts JSValue to char */
		size_t len;
		char *cstr;
		JSStringRef jsstr = JSValueToStringCopy(context, arguments[0], NULL);
		len = JSStringGetMaximumUTF8CStringSize(jsstr);
		cstr = g_new(char, len);
		JSStringGetUTF8CString(jsstr, cstr, len);
		g_print("%s\n\n", cstr);
		g_free(cstr);
		JSStringRelease(jsstr);
	}
	return JSValueMakeUndefined(context);
}

static void print_element_names(xmlNode * a_node) {
	xmlNode *cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			printf("node type: Element, name: %s\n", cur_node->name);
		}
		print_element_names(cur_node->children);
	}
}

static JSValueRef cb_sysclass_compile(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception) {
	/* At least, one argument must be received */
	if (argumentCount == 1 && JSValueIsString(context, arguments[0])) {
		/* Converts JSValue to char */
		size_t len;
		char *cstr;
		JSStringRef jsstr = JSValueToStringCopy(context, arguments[0], NULL);
		len = JSStringGetMaximumUTF8CStringSize(jsstr);
		cstr = g_new(char, len);
		JSStringGetUTF8CString(jsstr, cstr, len);

		//g_print("%s", cstr);

		xmlDoc *doc = NULL;
		xmlNode *root_element = NULL;
		LIBXML_TEST_VERSION
		doc = xmlReadMemory(cstr, strlen(cstr), "noname.xml", NULL, 0);

		g_free(cstr);
		JSStringRelease(jsstr);

		if (doc != NULL) {
			/*Get the root element node */
			root_element = xmlDocGetRootElement(doc);

			print_element_names(root_element);

			/* free the document */
			xmlFreeDoc(doc);

			/* Free the global variables that may have been allocated by the parser */
			xmlCleanupParser();
		} else {
			printf("error: could not parse XML\n");
		}

	}
	return JSValueMakeUndefined(context);
}

static JSValueRef cb_sysclass_gettext(JSContextRef context, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception) {
	/* At least, one argument must be received */
	if (argumentCount == 1 && JSValueIsString(context, arguments[0])) {
		/* Converts JSValue to char */
		size_t len;
		char *cstr;
		JSStringRef jsstr = JSValueToStringCopy(context, arguments[0], NULL);
		len = JSStringGetMaximumUTF8CStringSize(jsstr);
		cstr = g_new(char, len);
		JSStringGetUTF8CString(jsstr, cstr, len);
		JSStringRef translated_jsstr = JSStringCreateWithUTF8CString(_(cstr));
		g_free(cstr);
		JSStringRelease(jsstr);

		JSValueRef value = JSValueMakeString(context, translated_jsstr);
		JSStringRelease(translated_jsstr);
		return value;
	}

	JSStringRef jsstr = JSStringCreateWithUTF8CString("string");
	JSValueRef value = JSValueMakeString(context, jsstr);
	JSStringRelease(jsstr);
	return value;
}

/* Class method declarations */
static const JSStaticFunction sysclass_staticfuncs[] = {
	{ "print", cb_sysclass_print, kJSPropertyAttributeReadOnly },
	{ "compile", cb_sysclass_compile, kJSPropertyAttributeReadOnly },
	{ "gettext", cb_sysclass_gettext, kJSPropertyAttributeReadOnly },
	{ NULL, NULL, 0 }
};

JSObjectRef cb_sysclass_constructor(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	g_message("System class constructor");
	return (JSObjectRef)JSValueMakeNull(ctx);
}

static const JSClassDefinition sysclass_def =
{
	0,                       // version
	kJSClassAttributeNone,   // attributes
	"SysClass",              // className
	NULL,                    // parentClass
	NULL,                    // staticValues
	sysclass_staticfuncs,    // staticFunctions
	cb_sysclass_init,        // initialize
	cb_sysclass_finalize,    // finalize
	NULL,                    // hasProperty
	NULL,                    // getProperty
	NULL,                    // setProperty
	NULL,                    // deleteProperty
	NULL,                    // getPropertyNames
	NULL,                    // callAsFunction
	cb_sysclass_constructor, // callAsConstructor
	NULL,                    // hasInstance  
	NULL                     // convertToType
};

/* Callback - JavaScript window object has been cleared */
static void window_object_cleared_cb(WebKitWebView  *web_view, WebKitWebFrame *frame, gpointer context, gpointer window_object, gpointer user_data) {
	/* Add classes to JavaScriptCore */
	JSClassRef classDef = JSClassCreate(&sysclass_def);
	JSObjectRef classObj = JSObjectMake(context, classDef, context);
	JSObjectRef globalObj = JSContextGetGlobalObject(context);
	JSStringRef str = JSStringCreateWithUTF8CString("Sys");
	JSObjectSetProperty(context, globalObj, str, classObj, kJSPropertyAttributeNone, NULL);
}

int main(int argc, char* argv[]) {
	// Initialize GTK+
	gtk_init(&argc, &argv);

#ifdef USE_GETTEXT
	setlocale (LC_MESSAGES, "");
	setlocale (LC_CTYPE, "");
	setlocale (LC_COLLATE, "");
	textdomain ("gtk-blockly");
	bindtextdomain ("gtk-blockly", NULL);
#endif

	// Create an 800x600 window that will contain the browser instance
	GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

	// Set window title
	gtk_window_set_title(GTK_WINDOW(main_window), "Blockly Test");

	// Create a browser instance
	WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

	// Create a scrollable area, and put the browser instance into it
	GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

	// Set up callbacks so that if either the main window or the browser instance is
	// closed, the program will exit
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy_window), NULL);
	g_signal_connect(webView, "close-web-view", G_CALLBACK(cb_close_web_view), main_window);

	/* Connect the window object cleared event with callback */
	g_signal_connect (G_OBJECT(webView), "window-object-cleared", G_CALLBACK(window_object_cleared_cb), webView);

	// Create accelerators
	GtkAccelGroup * accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);

	// Elements for the menu
	GtkWidget * menu_bar = gtk_menu_bar_new();
	GtkWidget * menu_file = gtk_menu_new();
	GtkWidget * menu_file_file = gtk_menu_item_new_with_mnemonic(_("_File"));
	GtkWidget * menu_file_new = gtk_menu_item_new_with_mnemonic(_("_New"));
	GtkWidget * menu_file_open = gtk_menu_item_new_with_mnemonic(_("_Open"));
	GtkWidget * menu_file_exec = gtk_menu_item_new_with_mnemonic(_("_Execute"));
	GtkWidget * menu_file_sep = gtk_separator_menu_item_new();
	GtkWidget * menu_file_quit = gtk_menu_item_new_with_mnemonic(_("_Quit"));

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_file_file), menu_file);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_new);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_open);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_exec);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_sep);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_file_quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_file_file);

	gtk_widget_add_accelerator(menu_file_quit, "activate", accel_group, GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	g_signal_connect_swapped(G_OBJECT(main_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(menu_file_quit), "activate", G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect(G_OBJECT(menu_file_exec), "activate", G_CALLBACK(cb_execute), webView);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(box), menu_bar, FALSE, FALSE, 3);
	gtk_box_pack_start(GTK_BOX(box), scrolledWindow, TRUE, TRUE, 3);
	gtk_container_add(GTK_CONTAINER(main_window), box);

	// Set up callback to know the status of the page in the browser
	g_signal_connect(webView, "notify::load-status", G_CALLBACK(cb_load_status), NULL);

	// Load a web page into the browser instance
	webkit_web_view_load_uri(webView, "file://" DATA_DIR "/test.html");

	// Make sure that when the browser area becomes visible, it will get mouse
	// and keyboard events
	gtk_widget_grab_focus(GTK_WIDGET(webView));

	// Make sure the main window and all its contents are visible
	gtk_widget_show_all(main_window);

	// Run the main GTK+ event loop
	gtk_main();

	return EXIT_SUCCESS;
}
