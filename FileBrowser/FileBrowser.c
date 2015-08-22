//David Shagam
//Program 3 File Browser
//Unix 352

//precomipler
#define DEBUG 0
#define null NULL //because caps...//
//#define STARTDIR "/home/"
//2ndary enum that's nto an enum...
#define PATH 1
#define IMAGE 2
#define CRAWLED 3
#define COL2 4

//imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <gtk/gtk.h>
#include <math.h>

enum{
	NAME = 0,
	SIZE,
	MOD,
	FPATH,
	IMG,
	COL
};

//Globals for startdir and images.
void *gImg;
void *img1;
void *img2;
void *img3;
void *img4;
void *img5;
char *STARTDIR;

//name of file.
gint sort_string1 (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {
    
    gchar *name1, *name2;
    gtk_tree_model_get (model, a, NAME, &name1, -1);
    gtk_tree_model_get (model, b, NAME, &name2, -1);
    
    int order = strcasecmp(name1, name2);
    g_free(name1);
    g_free(name2);
    
    return order;
}

//file size
gint sort_num (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {
    
    guint name1, name2;
    gtk_tree_model_get (model, a, SIZE, &name1, -1);
    gtk_tree_model_get (model, b, SIZE, &name2, -1);
    
    if(name1 < name2){
    	return -1;
    } else if( name1 > name2){
    	return 1;
    } else {
    	return 0;
    }
}

//This is for year month date.
gint sort_string2 (GtkTreeModel *model,
                      GtkTreeIter *a, GtkTreeIter *b,
                      gpointer data) {
    
    gchar *name1, *name2;
    gtk_tree_model_get (model, a, MOD, &name1, -1);
    gtk_tree_model_get (model, b, MOD, &name2, -1);
    //grab all the data.
    int day1;
    int day2;
    int month1;
    int month2;
    int year1;
    int year2;

    sscanf(name1,"%d/%d/%d", &day1, &month1, &year1);
    sscanf(name2, "%d/%d/%d", &day2, &month2, &year2);
    //check everything.
    if(year1 > year2){
    	return 1;
    } else if (year1 < year2){
    	return -1;
    } else {
    	if(month1> month2){
    		return 1;
    	} else if (month1 < month2){
    		return -1;
    	} else {
    		if (day1 > day2) {
    			return 1;
    		} else if( day1 < day2){
    			return -1;
    		} else {
    			//If the year month and day are all the same check time do this the cheap way by doing a string comp.
    			return strcasecmp(name1, name2);
    		}
    	}
    }
}

//call back for destroying the entire thing.

static void destroy ( GtkWidget *widget, gpointer data){
	gtk_main_quit();
}

//This is ahelper function that adds files to the file viewer.
void addFiles(GtkListStore *store, char *path){
	gtk_list_store_clear(store);
	GtkTreeIter iter;
	char dirs[PATH_MAX+2];
	DIR *myDir;

	//DIR contains ino_t d_ino and char d_name[]
	//
	if((myDir = opendir(path)) != NULL){
		struct dirent *myEnt;
		while((myEnt = readdir(myDir)) !=NULL){
			struct stat statBuf;
			if(stat(myEnt -> d_name, &statBuf) != -1){
				if(S_ISREG(statBuf.st_mode) && myEnt->d_name[0] != '.'){
					gtk_list_store_append(store, &iter);
					memset(dirs, 0, sizeof(dirs));
					realpath(myEnt->d_name, dirs);
					char timest[30];
					time_t tempT = statBuf.st_mtim.tv_sec;
					struct tm *getTime = localtime(&tempT);
					strftime(timest, 30, "%D %r ", getTime);
					char *ext = strchr(myEnt->d_name, '.');
					GdkPixbuf *img;
					if(ext == NULL){
						if(statBuf.st_mode & S_IXUSR){
							img = img1;
						} else {
							img = img5;
						}
					} else if(!strcasecmp(ext,".c")){
						img = img3;
					} else if(!strcasecmp(ext,".o")){
						img = img4;
					} else if (!strcasecmp(ext, ".exe")){
						img = img1;
					} else if (!strcasecmp(ext,".txt")){
						img = img2;
					} else {
						img = img5;
					}
					gtk_list_store_set(store, &iter, NAME, myEnt->d_name, SIZE, statBuf.st_size, MOD, timest,FPATH,dirs,IMG,img,  -1);

				}
			}
		}
	}



}

//Helper function that adds directories to the folder viewer. Helps callback function.
void addDirs(GtkTreeStore* store, GtkTreeIter *pIter, char *path, GtkTreeModel *model){
	GtkTreeIter cIter;
	GtkTreeIter dIter;
	//GError *error = NULL;
	//GdkPixbuf *img = gdk_pixbuf_new_from_file_at_size("./foldericon.png",35,35, &error);
	while(gtk_tree_model_iter_children(model, &dIter, pIter)){
		gtk_tree_store_remove(store, &dIter);
	}

	char dirs[PATH_MAX+2];
	DIR *myDir;
	//DIR contains ino_t d_ino and char d_name[]
	//
	if((myDir = opendir(path)) != NULL){
		struct dirent *myEnt;
		while((myEnt = readdir(myDir)) !=NULL){
			struct stat statBuf;
			if(stat(myEnt -> d_name, &statBuf) != -1){
				if(S_ISDIR(statBuf.st_mode) && myEnt->d_name[0] != '.'){
					gtk_tree_store_append(store, &cIter, pIter);
					memset(dirs, 0, sizeof(dirs));
					realpath(myEnt->d_name, dirs);
					gtk_tree_store_set(store, &cIter, NAME, myEnt->d_name, PATH, dirs, IMAGE, gImg, CRAWLED,0, -1);

				}
			}
		}
	}


}

//Call back function for adding folders.
void item_selected (GtkWidget *dSelection, gpointer data) {
    
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(dSelection), 
        &model, &iter)) {
        
        gchar *path;
    	gint *crawl;
        gtk_tree_model_get (model, &iter, PATH, &path, CRAWLED, &crawl, -1);
        chdir(path);
        if(!crawl){
        	gtk_tree_store_set(GTK_TREE_STORE(data),&iter,CRAWLED, 1, -1);
        	addDirs(GTK_TREE_STORE(data), &iter, path, model);
    	}
    }
}

//Call back function for adding fles
void folder_selected(GtkWidget *selection, gpointer data){

    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection), 
        &model, &iter)) {
        
        gchar *path;
        gtk_tree_model_get (model, &iter, PATH, &path, -1);
        chdir(path);
        addFiles(GTK_LIST_STORE(data), path);
    }

}

//Populate text view for hex viewr.
void fillTextBuffer(GtkTextBuffer *buffer, char *path){
	GtkTextIter iter;
	FILE *input;
	GtkTextIter start;
	GtkTextIter end;
	//clear the buffer for a new file to get put in.
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	gtk_text_buffer_delete(buffer, &start, &end);

	//tags for formatting.
	GtkTextTag *greyBG = gtk_text_buffer_create_tag(buffer, NULL, "background", "grey","font","courier new", NULL);
	GtkTextTag *whiteBG = gtk_text_buffer_create_tag(buffer, NULL, "background", "white","font","courier new", NULL);

	//stat information
	struct stat statBuf;
	if((stat(path ,&statBuf)) == -1){
		printf("error on stat\n");
	} else {

		//calculate number of lines.
		int wholeLines = statBuf.st_size /16;
		int remLines = statBuf.st_size % 16;
		if(remLines >0){
			wholeLines++;
		}

		int curline = 0;
		int numPad = 0;
		if( statBuf.st_size > 0){
			numPad = floor(log(statBuf.st_size)/ log(16))+1;
		}


		if((input = fopen(path, "r")) == NULL){
			printf("Can't open it\n");
		} else {
			//get the position
			gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
			while(curline < wholeLines){
				char hexAddr[numPad+1];
				char hexVals[50];
				char asciVals[50];
				int vals[16];
				char cVals[16];
				sprintf(hexAddr,"%0*X ",numPad, curline*16);
				int i;
				//read in values 16 at a time.
				for(i = 0; i < 16; i++){
					vals[i] = getc(input);
					if(vals[i] == EOF){
						vals[i] = 0;
					}
					if(vals[i] < 32 || vals[i] == 127){
						cVals[i] = '.';
					}else if(vals[i]> 127){
						cVals[i] = '.';

					} else {
						cVals[i] = (char)vals[i];
					}
				}
				//format the output
				char *pad = "  ";
				sprintf(hexVals,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
				 vals[0], vals[1],vals[2],vals[3],vals[4],vals[5], vals[6], vals[7], vals[8], vals[9], vals[10], vals[11], vals[12], vals[13], vals[14],vals[15]);
				sprintf(asciVals, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
					cVals[0], cVals[1],cVals[2],cVals[3],cVals[4],cVals[5], cVals[6], cVals[7], cVals[8], cVals[9], cVals[10], cVals[11], cVals[12], cVals[13], cVals[14],cVals[15]);
				//insert into text view
				gtk_text_buffer_insert_with_tags(buffer, &iter,hexAddr, strlen(hexAddr) , greyBG, NULL);

				gtk_text_buffer_insert_with_tags(buffer, &iter, hexVals, strlen(hexVals), whiteBG, NULL);
				gtk_text_buffer_insert_with_tags(buffer, &iter, pad, strlen(pad), greyBG, NULL);
				gtk_text_buffer_insert_with_tags(buffer, &iter, asciVals, strlen(asciVals), whiteBG, NULL);
				curline++;
			}
		}
	}

}

//call back for file selection
void file_selected(GtkWidget *selection, gpointer data){

    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection), 
        &model, &iter)) {
        
    	gchar *path;
      	gtk_tree_model_get (model, &iter, FPATH, &path, -1);
       	//printf("the path is %s \n", path);
       	fillTextBuffer(GTK_TEXT_BUFFER(data), path);
     
    }

}

//for the tree view populat list
int populateDirTree(GtkTreeStore* store){
	GtkTreeIter iter[2];//assuming 100 folders deep is enough...
	GError *error = NULL;
	GdkPixbuf *img = gdk_pixbuf_new_from_file_at_size("./foldericon.png",35,35, &error);
	gImg = img;
	
	char dirs[PATH_MAX+2];
	DIR *myDir;
	//DIR contains ino_t d_ino and char d_name[]
	//
	if((myDir = opendir(STARTDIR)) != NULL){
		struct dirent *myEnt;
		realpath(STARTDIR, dirs);
		gtk_tree_store_append(store, &(iter[0]), NULL);
		int numLoops = 0;
		gtk_tree_store_set(store, &(iter[0]), NAME, STARTDIR , PATH, dirs, IMAGE, img , CRAWLED, 1, -1);
		chdir(STARTDIR);
		while((myEnt = readdir(myDir)) !=NULL){
			numLoops++;
			struct stat statBuf;
		
			if(stat(myEnt -> d_name, &statBuf) != -1){
				if(S_ISDIR(statBuf.st_mode) && myEnt->d_name[0] != '.'){
					gtk_tree_store_append(store, &(iter[1]), &(iter[0]));
					memset(dirs, 0, sizeof(dirs));
					realpath(myEnt->d_name, dirs);
					gtk_tree_store_set(store, &(iter[1]), NAME, myEnt->d_name, PATH, dirs, IMAGE, gImg, CRAWLED, 0 , -1);

				}
			}
		}
	}

	return 0;
}

//renders tree view of dirs
void renderTree(GtkWidget *treeview){
	GtkCellRenderer *renderer;
	GtkCellRenderer *rend2;
	GtkTreeViewColumn *column;
	rend2 = gtk_cell_renderer_pixbuf_new();
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_pack_start(column, rend2, FALSE);
	gtk_tree_view_column_set_attributes(column, rend2, "pixbuf", IMAGE, NULL);
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer, "text", NAME, NULL);
	gtk_tree_view_column_set_title(column, "Directory");
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

//populates the list for files in a dir
/*int populateList(GtkListStore* store){
	GtkTreeIter iter;
	
	return 0;
}*/

//Builds a renderer of a list of files in a dir.
void renderList(GtkWidget *treeview){
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkCellRenderer *rend2;
	rend2 = gtk_cell_renderer_pixbuf_new();
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_pack_start(column, rend2, FALSE);
	gtk_tree_view_column_set_attributes(column, rend2, "pixbuf", IMG, NULL);
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer, "text", NAME, NULL);
	gtk_tree_view_column_set_title(column, "File Name");
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

	//Sorting
	gtk_tree_view_column_set_sort_column_id(column, NAME);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Size", renderer, "text", SIZE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);


	//sorting
	gtk_tree_view_column_set_sort_column_id(column, SIZE);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);


	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Mod Date", renderer, "text", MOD, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	//sorting
	gtk_tree_view_column_set_sort_column_id(column, MOD);
    gtk_tree_view_column_set_sort_indicator(column, TRUE);
}

int main(int argc, char *argv[]){
	GtkWidget *window;
	struct passwd *usr = getpwuid(getuid());
	STARTDIR = usr->pw_dir;
	gtk_init (&argc, &argv);

	//Image init

	GError *error = NULL;
	
	GdkPixbuf *eimg = gdk_pixbuf_new_from_file_at_size("./exe.png",15,15, &error);
	error = NULL;
	GdkPixbuf *timg = gdk_pixbuf_new_from_file_at_size("./text.png",15,15, &error);
	error = NULL;
	GdkPixbuf *cimg = gdk_pixbuf_new_from_file_at_size("./c.png",15,15, &error);
	error = NULL;
	GdkPixbuf *oimg = gdk_pixbuf_new_from_file_at_size("./O.png",15,15, &error);
	error = NULL;
	GdkPixbuf *bimg = gdk_pixbuf_new_from_file_at_size("./blank.png",15,15, &error);
	error = NULL;
	img1 = eimg;
	img2 = timg;
	img3 = cimg;
	img4 = oimg;
	img5 = bimg;

	//Window init
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (window), "File Browser");
	gtk_container_set_border_width(GTK_CONTAINER (window), 10);
	gtk_widget_set_size_request (window, 750, 500);


	g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);
	//create panes
	GtkWidget *horizPane;
	horizPane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	GtkWidget *vertiPane;
	vertiPane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

	//Tree for directories.

	GtkWidget *folderViews;
	GtkTreeStore *folderStore = gtk_tree_store_new(COL2, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT);
	populateDirTree(folderStore);

	folderViews = gtk_tree_view_new();
	renderTree(folderViews);
	GtkWidget *folderScroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(folderScroll, 150, 250);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(folderScroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(folderScroll), folderViews);
	gtk_tree_view_set_model(GTK_TREE_VIEW(folderViews), GTK_TREE_MODEL(folderStore));
	g_object_unref(folderStore);
	//Tree dSelection
	GtkTreeSelection *dSelection;
	dSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(folderViews));
	
	//char dataList[COL][100];


	//File browsing in a direcotry.
	GtkWidget *fileViews;
	GtkListStore *store = gtk_list_store_new(COL, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	//populateList(store);
	fileViews = gtk_tree_view_new();
	renderList(fileViews);
	//Sorting stuff
	GtkTreeSortable *sortable = GTK_TREE_SORTABLE(store);
	gtk_tree_sortable_set_sort_func(sortable, NAME, sort_string1, GINT_TO_POINTER(NAME), NULL);
	gtk_tree_sortable_set_sort_column_id(sortable, NAME, GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func(sortable, SIZE, sort_num, GINT_TO_POINTER(SIZE), NULL);
	gtk_tree_sortable_set_sort_column_id(sortable, SIZE, GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func(sortable, MOD, sort_string2, GINT_TO_POINTER(NAME),NULL);
	gtk_tree_sortable_set_sort_column_id(sortable, MOD, GTK_SORT_ASCENDING);

	GtkWidget *fileScroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(fileScroll,250, 100);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fileScroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(fileScroll), fileViews);


	gtk_tree_view_set_model (GTK_TREE_VIEW(fileViews), GTK_TREE_MODEL(store));

	//File Selection
	GtkTreeSelection *fSelection;
	fSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(fileViews));
	g_object_unref(store);

	
	//HexViewer

	GtkWidget *textView;
	textView = gtk_text_view_new();
	GtkTextBuffer *tBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textView));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);


	GtkWidget *hexScroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(hexScroll, 250, 250);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(hexScroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(hexScroll), textView);

	//Callback connections
	g_signal_connect(G_OBJECT(dSelection), "changed", G_CALLBACK(item_selected), G_OBJECT(folderStore));
	g_signal_connect(G_OBJECT(dSelection), "changed", G_CALLBACK(folder_selected), G_OBJECT(store));
	g_signal_connect(G_OBJECT(fSelection), "changed", G_CALLBACK(file_selected), G_OBJECT(tBuff));

	//Pack everything togethere.
	gtk_paned_pack1(GTK_PANED (vertiPane), fileScroll, TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED (vertiPane), hexScroll, TRUE, FALSE);


	gtk_paned_pack1(GTK_PANED (horizPane), folderScroll, TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED (horizPane), vertiPane, TRUE, FALSE);







	gtk_container_add(GTK_CONTAINER(window), horizPane);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;

}