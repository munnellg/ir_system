#include "irapp.h"
#include <stdio.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
	return g_application_run(G_APPLICATION(ir_app_new()), argc, argv);
}
