#include "irapp.h"
#include "irappwin.h"
#include "irappvidx.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

enum {
	COL_TERM,
	COL_FREQUENCY,
	NUM_COLS
};

struct _IrAppVIdx {
	GtkDialog parent;
};

struct _IrAppVIdxClass {
	GtkDialogClass parent_class;
};

typedef struct _IrAppVIdxPrivate IrAppVIdxPrivate;

G_DEFINE_TYPE(IrAppVIdx, ir_app_vidx, GTK_TYPE_DIALOG)

static void ir_app_vidx_dispose(GObject *object) {
	G_OBJECT_CLASS(ir_app_vidx_parent_class)->dispose(object);
}
/*
static int
add_posting_to_tree_model(char *key, IPosting *val, gpointer user_data) {
	GtkTreeStore *treestore;
	GtkTreeIter   child;
	
	treestore = (GtkTreeStore *) user_data;
	
	gtk_tree_store_prepend(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
										 COL_TERM, "Doc ID",
										 COL_FREQUENCY, val->doc_id,
										 -1);
	gtk_tree_store_prepend(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
										 COL_TERM, "Doc Length",
										 COL_FREQUENCY, posting->doc_data->doc_length,
										 -1);
	gtk_tree_store_prepend(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
										 COL_TERM, "Doc Term Frequence",
										 COL_FREQUENCY, posting->term_freq,
										 -1);
	return 0;
	}*/

static GtkTreeModel *
create_and_fill_index_model () {
	GtkTreeStore *treestore;
	GtkTreeIter  toplevel, child;
	GTree *postings;
	GList *terms, *t;
	IPostingList *pl;
	
	terms = i_indexer_get_indexed_terms();
	treestore = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

	for(t=terms; t!=NULL; t=t->next) {
		pl = i_indexer_get_term_posting_list((char*)t->data);
		
		gtk_tree_store_prepend(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel,
											 COL_TERM, i_posting_list_get_token(pl),
											 COL_FREQUENCY, i_posting_list_get_freq(pl),
											 -1);

	/*		postings = i_posting_list_get_postings(pl);
		g_tree_foreach(postings, (GTraverseFunc) add_posting_to_tree_model, GtkTreeStore);*/
		}


	return GTK_TREE_MODEL(treestore);
}

static GtkWidget *
create_index_view_and_model() {
	GtkTreeViewColumn *col;
	GtkCellRenderer   *renderer;
	GtkWidget         *view;
	GtkTreeModel      *model;

	view = gtk_tree_view_new();

	gtk_tree_view_set_grid_lines (GTK_TREE_VIEW(view), GTK_TREE_VIEW_GRID_LINES_BOTH);
	model = create_and_fill_index_model();
	

	col = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new();
	
	gtk_tree_view_column_set_title(col, "Term");
	gtk_tree_view_column_set_resizable(col, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COL_TERM);


	col = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new();
	
	gtk_tree_view_column_set_title(col, "Frequencey");
	gtk_tree_view_column_set_resizable(col, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COL_FREQUENCY);


	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
	g_object_unref(model);

	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)), GTK_SELECTION_NONE);
	
	return view;
}

static void ir_app_vidx_init(IrAppVIdx *vidx) {
	GtkWidget *tree, *container, *root;

	tree = create_index_view_and_model();
	gtk_widget_set_visible(tree, TRUE);
	gtk_widget_set_hexpand(tree, TRUE);
	gtk_widget_set_vexpand(tree, TRUE);

	container = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_hexpand(container, TRUE);
	gtk_widget_set_vexpand(container, TRUE);
	gtk_container_add(GTK_CONTAINER(container), tree);
	gtk_widget_set_visible(container, TRUE);
	
	root = gtk_dialog_get_content_area(GTK_DIALOG(vidx));
	gtk_container_add(GTK_CONTAINER(root), container);

	gtk_window_set_title(GTK_WINDOW(vidx), "Index");
	gtk_window_set_modal(GTK_WINDOW(vidx), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(vidx), TRUE);
	gtk_window_set_default_size(GTK_WINDOW(vidx), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	gtk_widget_set_visible(container, TRUE);
}

static void ir_app_vidx_class_init(IrAppVIdxClass *class) {
	G_OBJECT_CLASS(class)->dispose = ir_app_vidx_dispose;
}

IrAppVIdx *ir_app_vidx_new(IrAppWindow *win) {
	return g_object_new(IR_APP_VIDX_TYPE, "transient-for", win, NULL, "use-header-bar", TRUE, NULL);
}
