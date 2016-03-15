# OpenCv-Gaze-tracking
// eyeface8.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include <stdio.h>

#define  TPL_WIDTH       10     /* template width       */
#define  TPL_HEIGHT      10     /* template height      */
#define  WINDOW_WIDTH    20     /* search window width  */
#define  WINDOW_HEIGHT   20   /* search window height */
#define  THRESHOLD       1.7
IplImage *framein, *frame, *frame1, *frame2, *tpl, *tplnew, *tplnew1, *tpl1, *tm, *tm1, *fm, *em, *em1, *eye1, *eye2, *em2,*img1, *full, *fruits, *prev1, *prev2, *curr1, *curr2 ;
IplImage *velocityX1 , *velocityY1, *velocityX2, *velocityY2;
CvSize size1,size2;
uchar *data;

CvHaarClassifierCascade *cascade_f;
CvHaarClassifierCascade *cascade_e;
CvMemStorage *storage;
CvMemStorage *storage1;
CvMemStorage *storage2;
CvSeq* results;
CvSeq* results2;
CvFileStorage* fs;
CvFileStorage* fs1;
CvFileStorage* vel;
CvFileStorage* vel1;
CvFileStorage* eyefirst;
CvFileStorage* eyesecond;
CvVideoWriter *writer = 0;

 int object_x0, object_y0, is_tracking = 0;
 int object_x02, object_y02;
 int ex,ey,eyex,eyey;
 int coord[10];
 int coord2[10];
 int preveye[10];
int win_x0, win_y0, win_x02, win_y02;
CvRect e_rect;
	CvRect eye1rect;
	CvRect eye2rect;
	float* p;
	int xc,yc,rad;
	float* p2;
int xc2,yctwo,radtwo;
int eye1width,eye1height;
int eye2width,eye2height;

int fullx1,fully1,fullx2,fully2,fullx1p, fully1p, fullx2p, fully2p;
CvKalman *m_pKalmanFilter; 
		CvRandState rng; 
		CvMat* state; 
		CvMat* process_noise; 
		CvMat* measurement; 
		CvMat* measurement_noise; 
		int dynam_params = 4;	// x,y,dx,dy
		int measure_params = 2;

//void mouseHandler( int event, int x, int y, int flags, void *param );
void trackObject();
IplImage* cropImage(const IplImage *img, const CvRect region);
void detectEyes(IplImage *img); 
 
/* main code */
int _tmain(int argc, _TCHAR* argv[])
{
    CvCapture   *capture;
	CvCapture *capture1;
	CvFileStorage* fs;
	CvFileStorage* fs1;
	CvFileStorage* vel;
	CvFileStorage* vel1;

    int         key=' ', key1=' ';
	char      *filename = "haarcascade_frontalface_alt.xml";
    char      *filename1 = "haarcascade_eye.xml";
	/* load the classifier 
       note that I put the file in the same directory with
       this code */
    cascade_f = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
    cascade_e = ( CvHaarClassifierCascade* )cvLoad( filename1, 0, 0, 0 );

	printf("%d  %d  window\n\n",cascade_f->orig_window_size.width,cascade_f->orig_window_size.height);  
	printf("%d  %d %f count\n\n",cascade_f->count, cascade_f->flags, cascade_f->scale);  
	printf("%d stage\n\n",cascade_f->stage_classifier->count);  
	printf("%d width \n\n",cascade_f->stage_classifier->classifier->haar_feature->rect->r.width); 
	printf("%d height \n\n",cascade_f->stage_classifier->classifier->haar_feature->rect->r.height); 

for( int k = 0; k < CV_HAAR_FEATURE_MAX; k++ )
{
	printf("%d width \n\n",cascade_f->stage_classifier->classifier->haar_feature->rect->r.width); 
	printf("%d height \n\n",cascade_f->stage_classifier->classifier->haar_feature->rect->r.height); 

}
	printf("%d  %d  eye\n\n",cascade_e->orig_window_size.width,cascade_e->orig_window_size.height); 

	
	int isColor = 1;
int fps     = 25;  // or 30
int frameW  = 640; // 744 for firewire cameras
int frameH  = 480; // 480 for firewire cameras
writer=cvCreateVideoWriter("out6.avi",CV_FOURCC('P','I','M','1'),fps,cvSize(frameW,frameH),isColor);	
  
	 /* setup memory buffer; needed by the face detector */
	 storage  = cvCreateMemStorage( 0 );
	 storage1 = cvCreateMemStorage( 0 );
	 storage2 = cvCreateMemStorage( 0 );
   
    /* initialize camera */
    capture = cvCaptureFromCAM( 0 );
 
    /* always check */
    if( !capture ) return 1;
 
    /* get video properties, needed by template image */
    frame = cvQueryFrame( capture);
	printf("depth and nchannels %d , %d", frame->depth, frame->nChannels);

	if ( !frame ) return 1;
	// cvShowImage("frame..in", framein);
	//frame = cvCreateImage( cvSize( framein->width, framein->height ),IPL_DEPTH_8U, 1 );
	//cvConvertImage(framein,frame,0); // cimg -> gimg
   	 //cvNamedWindow( "frame..in", 1 );
     cvNamedWindow( "frame..", 1 );  	 
	 cvShowImage("frame..", frame);
    /* create template image */
	 tpl = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),frame->depth, frame->nChannels  );
	tplnew = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),frame->depth, frame->nChannels  );
	tplnew1 = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),frame->depth, frame->nChannels  );
	/* create template image */
    tpl1 = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),frame->depth, frame->nChannels );

	
	prev1 = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),IPL_DEPTH_8U, 1);
	prev2 = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),IPL_DEPTH_8U, 1 );
	
	curr1 = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),IPL_DEPTH_8U, 1);
	curr2 = cvCreateImage( cvSize( TPL_WIDTH, TPL_HEIGHT ),IPL_DEPTH_8U, 1 );
	printf("tplmain...%d %d\n",tpl->width,tpl->height);
	printf("tplmain...%d %d\n",tpl1->width,tpl1->height);
    
    /* create image for template matching result */
    tm = cvCreateImage( cvSize( WINDOW_WIDTH  - TPL_WIDTH +1,
                                WINDOW_HEIGHT - TPL_HEIGHT +1),
								IPL_DEPTH_32F, 1 );
	printf("tmmain...%d %d\n",tm->width,tm->height);

	tm1 = cvCreateImage( cvSize( WINDOW_WIDTH - TPL_WIDTH +1,
                                WINDOW_HEIGHT - TPL_HEIGHT +1),
								IPL_DEPTH_32F, 1  );
	printf("tm1..main...%d %d\n",tm1->width,tm1->height);
    
    /* create a window and install mouse handler */   
	 cvNamedWindow( "video1", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "video2", 1 );
	cvNamedWindow( "video3", 1 );
	cvNamedWindow( "video4", 1 );
	cvNamedWindow( "video5", 1 );
	cvNamedWindow( "video6", 1 );
	cvNamedWindow( "video7", 1 );
	cvNamedWindow( "video8", 1 );
	cvNamedWindow( "video9", 1 );
	cvNamedWindow( "full2", 1 );
	
	cvNamedWindow( "fruits", 0 );
	
	frame1 = cvCreateImage( cvSize( frame->width, frame->height ), frame->depth, frame->nChannels );
	cvCopy(frame,frame1,0);
printf("%d   %d img ",frame->width, frame->height);

	 detectEyes(frame1);
   
	 vel = cvOpenFileStorage("first.txt",0,CV_STORAGE_APPEND);
	 vel1 = cvOpenFileStorage("second.txt",0,CV_STORAGE_APPEND);

	
   //while( key != 'q' ) {
	for(int cnt=1; cnt<=8; cnt++){
        /* get a frame */
		printf("cnt....................%d\n",cnt); 
        frame = cvQueryFrame( capture );
	//	framein = cvQueryFrame( capture);
	//	frame = cvCreateImage( cvSize( framein->width, framein->height ),IPL_DEPTH_8U, 1 );
	//	cvConvertImage(framein,frame,0); // cimg -> gimg
   	//	cvShowImage("framein..", framein);
		cvShowImage("frame..", frame);
        /* always check */
       // if( !frame ) break;
 
        /* 'fix' frame */
        //cvFlip( frame, frame, 1 );
        frame->origin = 0;
        
        /* perform tracking if template is available */
		if( is_tracking ) {trackObject(); }      
        /* display frame */
		
		//cvShowImage("full2", frame); 
		
         /* exit if user press 'q' */
        key = cvWaitKey(0);
    } 
	
    /* free memory */
    cvDestroyWindow( "video1" );
	cvDestroyWindow( "video2" );
	cvDestroyWindow( "video3" );
	cvDestroyWindow( "video4" );
	cvDestroyWindow( "video5" );
	cvDestroyWindow( "video6" );
	cvDestroyWindow( "video7" );
	cvDestroyWindow( "video8" );
	cvDestroyWindow( "video9" );
	cvDestroyWindow( "full2" );
	cvDestroyWindow( "fruits" );
	
    cvReleaseCapture( &capture );
	cvReleaseHaarClassifierCascade( &cascade_f );
	cvReleaseHaarClassifierCascade( &cascade_e );
    cvReleaseMemStorage( &storage );
	 cvReleaseMemStorage( &storage1 );
	 cvReleaseMemStorage( &storage2 );
	 cvReleaseVideoWriter(&writer);
    cvReleaseImage( &tpl );
    cvReleaseImage( &tm );
	
    return 0;
}
 
// Returns a new image that is a cropped version (rectangular cut-out)
// of the original image.
IplImage* cropImage(const IplImage *img, const CvRect region)
{
	IplImage *imageCropped;
	CvSize size;

	if (img->width <= 0 || img->height <= 0
		|| region.width <= 0 || region.height <= 0) {
		//cerr << "ERROR in cropImage(): invalid dimensions." << endl;
		exit(1);
	}

	if (img->depth != IPL_DEPTH_8U) {
		//cerr << "ERROR in cropImage(): image depth is not 8." << endl;
		exit(1);
	}

	// Set the desired region of interest.
	cvSetImageROI((IplImage*)img, region);
	// Copy region of interest into a new iplImage and return it.
	size.width = region.width;
	size.height = region.height;
	imageCropped = cvCreateImage(size, IPL_DEPTH_8U, img->nChannels);
	cvCopy(img, imageCropped);	// Copy just the region.

	return imageCropped;
}

/* mouse handler */
/*void mouseHandler( int event, int x, int y, int flags, void *param )
{
   // user clicked the image, save subimage as template 
    if( event == CV_EVENT_LBUTTONUP ) {
        object_x0 = x - ( TPL_WIDTH  / 2 );
        object_y0 = y - ( TPL_HEIGHT / 2 ); 
        
        cvSetImageROI( frame, 
                       cvRect( object_x0, 
                               object_y0, 
                               TPL_WIDTH, 
                               TPL_HEIGHT ) );
        cvCopy( frame, tpl, NULL );
        cvResetImageROI( frame );
 
        // template is available, start tracking! 
        fprintf( stdout, "Template selected. Start tracking... \n" );
        is_tracking = 1;
    }
}*/

void detectEyes(IplImage *img) 
{	
printf("inside function.......\n");
full= cvCreateImage( cvSize( img->width, img->height ), 
                         img->depth, img->nChannels );
cvCopy(img,full,0);

//cvShowImage( "video7", full);
	int i,j,k;
    /* detect faces */
    CvSeq *faces = cvHaarDetectObjects(
        img,            /* the source image */ 
        cascade_f,      /* the face classifier */ 
        storage,        /* memory buffer, created with cvMemStorage */
        1.1, 3, 0,      /* special parameters, tune for your app */ 
        cvSize(40, 40)  /* minimum detection scale */ 
    );
 printf("%d  %d  window after\n\n",cascade_f->orig_window_size.width,cascade_f->orig_window_size.height);  
	printf("%d  %d %fcount after\n\n",cascade_f->count, cascade_f->flags, cascade_f->scale);  

	printf("%d stage\n\n",cascade_f->stage_classifier->count);  
	printf("%d classifier \n\n",cascade_f->stage_classifier->classifier->count); 
    /* return if not found */
    if (faces->total == 0) return;
	/* get the first detected face */
    CvRect *face = (CvRect*)cvGetSeqElem(faces, 0);
 
	int prevfacex=face->x;
	int prevfacey=face->y;
	printf("%d  %d prevface\n", prevfacex, prevfacey);

    CvRect f_rect;
    f_rect = cvRect(
    face->x,
    face->y + (face->height/5.5),
    face->width,
    face->height/3.0
); 

 
 fm=cropImage(img, f_rect);
 
 cvShowImage("video1", fm);

 int facex=face->x;
 int facey=face->height/5.5;

 img1 = cvCreateImage( cvSize( fm->width, fm->height ), fm->depth, fm->nChannels );
  cvCopy(fm,img1,0); 

 
	    	   /* detect the eyes */
    CvSeq *eyes = cvHaarDetectObjects( 
        fm,            /* the source image, with the estimated location defined */ 
        cascade_e,      /* the eye classifier */ 
        storage,        /* memory buffer */
        1.15, 3, 0,     /* tune for your app */ 
        cvSize(25,15)  /* minimum detection scale */
    );

	 /* return if not found */
    if (eyes->total == 0) return;
	int e1,e2,e3,e4;
	int e5,e6,e7,e8;

    /* draw a rectangle for each detected eye */
    
        /* get one eye */
        CvRect *eye = (CvRect*)cvGetSeqElem(eyes, 0);
		CvRect *eyetwo = (CvRect*)cvGetSeqElem(eyes, 1);

		 e1=eye->x; e2=eye->y; 
e3=eye->width; e4=eye->height;
printf("%d %d %d %d..........e1234\n" , e1,e2,e3,e4);
eyefirst = cvOpenFileStorage("eye1.txt",0,CV_STORAGE_APPEND);
		cvWriteInt( eyefirst, "e1", e1);
		cvWriteInt( eyefirst, "e2", e2 );
		cvWriteInt( eyefirst, "e3", e3);
		cvWriteInt( eyefirst, "e4", e4);
printf("rect1......");
        /* draw a red rectangle 
        cvRectangle(
            fm, 
            cvPoint(eye->x, eye->y), 
            cvPoint(eye->x + eye->width, eye->y + eye->height),
            CV_RGB(255, 0, 0), 
            1, 8, 0
        );*/

 e5=eyetwo->x; e6=eyetwo->y; 
e7=eyetwo->width; e8=eyetwo->height;
printf("%d %d %d %d..........e5678\n", e5,e6,e7,e8);
eyesecond = cvOpenFileStorage("eye2.txt",0,CV_STORAGE_APPEND);
		cvWriteInt( eyesecond, "e5", e5);
		cvWriteInt( eyesecond, "e6", e6 );
		cvWriteInt( eyesecond, "e7", e7);
		cvWriteInt( eyesecond, "e8", e8);	

eye1rect = cvRect(
    e1,
	e2,
	e3,
	e4
); 

printf("%d %d %d %d eye1rect\n",e1,e2,e3,e4);
 eye1=cropImage(fm,eye1rect);
 
cvShowImage( "video2", fm);
 printf("1.......");

cvShowImage( "video3", eye1);
printf("2.......");

eye1width  = eye1->width;
eye1height = eye1->height;
 
IplImage *em1 = cvCreateImage( cvSize( eye1width, eye1height ), IPL_DEPTH_8U, 1 );
cvCvtColor(eye1, em1, CV_BGR2GRAY);
//cvSmooth(em1, em1, CV_GAUSSIAN, 3,3 );

cvShowImage( "video4", em1 );
printf("3.......");

results= cvHoughCircles(em1,storage1,CV_HOUGH_GRADIENT,1, em1->height/4, 10,10,10, 15 );

CvRect c1rect,c2rect;

for( int j = 0;j < 1; j++ ) {
p = (float*) cvGetSeqElem( results, j );
CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
xc=cvRound( p[0]); yc=cvRound( p[1]); rad= cvRound(p[2]);
printf("%d %d %d center and radius\n", xc, yc, rad);
coord[j,1]=xc; coord[j,2]=yc; coord[j,3]=rad;
printf("%d %d %d  coord\n", coord[j,1], coord[j,2], coord[j,3]);

cvCircle(
em1,
pt,
cvRound( p[2] ),
CV_RGB(0xff,0xff,0xff)
);

 /* draw a red rectangle */
        cvRectangle(
            em1, 
            cvPoint(xc-5, yc-5), 
            cvPoint(xc+5, yc+5),
            CV_RGB(255, 255,255), 
            1, 8, 0
        );

}

cvNamedWindow( "cvHoughCircles1", 1 );
cvShowImage("cvHoughCircles1", em1);
printf("4.......");

//------------------------------------second eye
//second eye

eye2rect = cvRect(
    e5,e6,e7,e8
); 


printf("%d %d %d %d eye2rect\n", e5,e6,e7,e8);

 
 eye2=cropImage(img1,eye2rect);
 
 cvShowImage( "video5", eye2);


eye2width  = eye2->width;
eye2height = eye2->height;
 

IplImage *em2 = cvCreateImage( cvSize( eye2width, eye2height ), IPL_DEPTH_8U, 1 );
cvCvtColor(eye2, em2, CV_BGR2GRAY);
//cvSmooth(em2, em2, CV_GAUSSIAN, 3,3 );

cvShowImage( "video6", em2 );

results2= cvHoughCircles(em2,storage2,CV_HOUGH_GRADIENT,1, em2->height/4, 10,10,10, 15 );
 
for( int k = 0; k < 1; k++ ) {
p2 = (float*) cvGetSeqElem( results2, k );
CvPoint pt2 = cvPoint( cvRound( p2[0] ), cvRound( p2[1] ) );
xc2=cvRound( p2[0]); 
yctwo=cvRound( p2[1]); 
radtwo= cvRound(p2[2]);
printf("%d %d %d center and radius", xc2, yctwo, radtwo);
coord2[k,1]=xc2; coord2[k,2]=yctwo; coord2[k,3]=radtwo;
printf("%d %d %d  coord2\n", coord2[k,1], coord2[k,2], coord2[k,3]);

cvCircle(
em2,
pt2,
cvRound( p2[2] ),
CV_RGB(0xff,0xff,0xff)
);
/* draw a red rectangle */
        cvRectangle(
            em2, 
            cvPoint(xc2-5, yctwo-5), 
            cvPoint(xc2+5, yctwo+5),
            CV_RGB(255, 255, 255), 
            1, 8, 0
        );
}
cvNamedWindow( "cvHoughCircles2", 1 );
cvShowImage("cvHoughCircles2", em2);

//----------------------------------------------------
fullx1=xc+e1+prevfacex;
fully1=yc+e2+facey+prevfacey;
printf("%d %d fullx1,y1\n",fullx1,fully1);

fullx2=xc2+e5+prevfacex;
fully2=yctwo+e6+facey+prevfacey;
printf("%d %d fullx2,y2\n",fullx2,fully2);

		
		object_x0 = fullx1 - ( TPL_WIDTH  / 2 );
        object_y0 = fully1 - ( TPL_HEIGHT / 2 ); 
        
        cvSetImageROI( full, 
                       cvRect( object_x0, 
                               object_y0, 
                               TPL_WIDTH, 
                               TPL_HEIGHT ) );
        cvCopy( full, tpl, NULL );
		cvShowImage( "video8", tpl);
		cvConvertImage(tpl,prev1,0);
		cvConvertImage(tpl,curr1,0);
		cvNamedWindow( "video10", 1 );
		cvShowImage("video10", prev1);
		cvResetImageROI( full ); 

		object_x02 = fullx2 - ( TPL_WIDTH  / 2 );
        object_y02 = fully2 - ( TPL_HEIGHT / 2 ); 
        
        cvSetImageROI( full, 
                       cvRect( object_x02, 
                               object_y02, 
                               TPL_WIDTH, 
                               TPL_HEIGHT ) );
        cvCopy( full, tpl1, NULL );
		cvShowImage( "video9", tpl1);
		cvConvertImage(tpl1,prev2,0);
		cvConvertImage(tpl1,curr2,0);
		cvNamedWindow( "video11", 1 );
		cvShowImage("video11", prev2);
        cvResetImageROI( full ); 
 
 
	cvRectangle(
            full, 
            cvPoint(fullx1-5,fully1-5), 
            cvPoint(fullx1+5,fully1+5),
            CV_RGB(255, 255, 255), 
            1, 8, 0
        );
 cvRectangle(
            full, 
            cvPoint(fullx2-5,fully2-5), 
            cvPoint(fullx2+5,fully2+5),
            CV_RGB(255, 255, 255), 
            1, 8, 0
        );

cvNamedWindow( "full1", 1 );
cvShowImage("full1", full);
  
 /* template is available, start tracking! */
        fprintf( stdout, "Template selected. Start tracking... \n" );
        is_tracking = 1;
}
/* end of detectEyes() */
 
/* track object */
void trackObject()
{
    int i,j;
	CvPoint minloc, maxloc;
    double  minval, maxval;
	CvPoint minloc1, maxloc1;
    double  minval1, maxval1;
	
 /* setup position of search window */
    win_x0 = fullx1 -  ( ( WINDOW_WIDTH  - TPL_WIDTH  ) / 2 );
    win_y0 = fully1 -  ( ( WINDOW_HEIGHT - TPL_HEIGHT ) / 2 );
    printf("win...%d %d\n",win_x0,win_y0);

	win_x02 = fullx2 -  ( ( WINDOW_WIDTH  - TPL_WIDTH  ) / 2 );
    win_y02 = fully2 -  ( ( WINDOW_HEIGHT - TPL_HEIGHT ) / 2 );
    printf("win2...%d %d\n",win_x02,win_y02);
      

	 /*
     * Ooops, some bugs here.
     * If the search window exceed the frame boundaries,
     * it will trigger errors.
     *
     * Add some code to make sure that the search window 
     * is still within the frame.
     */
    
    /*---------- search object in search window */
    cvSetImageROI( frame, 
                   cvRect( win_x0, 
                           win_y0, 
                           WINDOW_WIDTH, 
                           WINDOW_HEIGHT ) );

	printf("\t%d %d....\n", frame->width,frame->height);
	printf("\t%d %d\n",tpl->width,tpl->height);
	
    cvMatchTemplate( frame, tpl, tm, CV_TM_CCOEFF);
	//cvNormalize(tm,tm,1,0,CV_MINMAX,NULL);
	cvNamedWindow("searchwindow", 1);
	cvShowImage("searchwindow", frame);

	cvNamedWindow("tpl", 1);
	cvShowImage("tpl", tpl);

	cvNamedWindow("tm", 1);
	cvShowImage("tm", tm);
	size1 = cvGetSize(tpl);
	velocityX1 = cvCreateImage(size1, IPL_DEPTH_32F, 1);
	velocityY1 = cvCreateImage(size1, IPL_DEPTH_32F, 1);
	//cvConvertImage(tpl,curr1,0);
	
	printf("size1...%d\n",size1);
	printf("curr1...%d %d\n",curr1->width,curr1->height);
	cvCalcOpticalFlowLK(prev1, curr1, cvSize(3, 3),velocityX1, velocityY1);
	
	cvNamedWindow("Horizontal", 1);
	cvShowImage("Horizontal", velocityX1);

	cvNamedWindow("Vertical", 1);
	cvShowImage("Vertical", velocityY1);
	printf("curr width, height %d %d\n", curr1->width, curr1->height);
	vel = cvOpenFileStorage("first.txt",0,CV_STORAGE_APPEND);
for (int x=0; x<curr1->width; x=x+2) {
  for (int y=0; y<curr1->height; y=y+2) {
    double vel_x_here = cvGetReal2D( velocityX1, y, x);
    double vel_y_here = cvGetReal2D( velocityY1, y, x);
    cvLine( curr1, cvPoint(x, y), cvPoint(x+vel_x_here ,y+vel_y_here), cvScalarAll(255));
	printf("%f , %f\n",vel_x_here,vel_y_here);
	
		cvWriteReal( vel, "velx",  vel_x_here);
		cvWriteReal( vel, "vely", vel_y_here );
  }
}
	cvNamedWindow("result", 1);
	cvShowImage("result", curr1);

cvCopy(curr1,prev1,0);	
	/*CvScalar s;
		for (i=0;i<10;i++){
			for(j=0; j<10; j++){
		s=cvGet2D(tpl,i,j); // get the (i,j) pixel value
		printf("intensity=%f\n",s.val[0]);
			}
		}*/
	printf("tpl...%d %d\n",tpl->width,tpl->height);
	printf("tm...%d %d\n",tm->width,tm->height);

    cvMinMaxLoc( tm, &minval, &maxval, &minloc, &maxloc, 0 );
	printf("minval,maxval...%f %f\n",minval,maxval);
    cvResetImageROI( frame );
    
	    /* if object found... */
    if( minval <= THRESHOLD ) {
        /* save object's current location */
        fullx1 = win_x0 + maxloc.x;
        fully1 = win_y0 + maxloc.y;
		printf("%d %d ...win_x0, win_y0...threshold\n",win_x0,win_y0);
		printf("%d %d...maxloc.x,maxloc.y", maxloc.x, maxloc.y);
		printf("%d %d ...fullx1,fully1...threshold\n",fullx1,fully1);
		
		fs = cvOpenFileStorage(
			"rectangle.txt",
			0,
			CV_STORAGE_APPEND
			);
		cvWriteInt( fs, "e1",fullx1 );
		cvWriteInt( fs, "e2",fully1 );
 
        /* and draw a box there */
		printf("draw box");
		//--------------
       cvRectangle( frame,
                     cvPoint( fullx1-5, fully1-5 ),
                     cvPoint( fullx1+5, fully1+5),
                     cvScalar( 0, 0, 255, 0 ), 1, 0, 0 );

		//------------------
		
		//cvResizeWindow("full2",400,400);
		//cvMoveWindow( "full2", 0, 0 );
		cvNamedWindow( "full2", 1 );
		cvShowImage("full2", frame);
		cvWriteFrame(writer,frame); 
		printf("draw box over");
    } else {
        /* if not found... */
        fprintf( stdout, "Lost object.\n" );
        is_tracking = 0;
    }
	
		int obj_x0, obj_y0;

		obj_x0 = fullx1 - ( TPL_WIDTH  / 2 );
        obj_y0 = fully1 - ( TPL_HEIGHT / 2 ); 
        
        cvSetImageROI( frame, 
                       cvRect( obj_x0, 
                               obj_y0, 
                               TPL_WIDTH, 
                               TPL_HEIGHT ) );
        cvCopy( frame, tplnew, NULL );
		cvConvertImage(tplnew,curr1,0);
		cvNamedWindow("curr1", 1);
		cvShowImage("curr1", curr1);
		cvResetImageROI( frame );
//second eye------------------------------------

	/* search object in search window */
    cvSetImageROI( frame, 
                   cvRect( win_x02-5, 
                           win_y02-5, 
                           WINDOW_WIDTH, 
                           WINDOW_HEIGHT ) );

	printf("\t%d %d....", frame->width,frame->height);
	printf("\t%d %d\n",tpl1->width,tpl1->height);

    cvMatchTemplate( frame, tpl1, tm1, CV_TM_CCOEFF);
	printf("tpl1...%d %d\n",tpl1->width,tpl1->height);
	printf("tm...%d %d\n",tm1->width,tm1->height);
//--------------
size2 = cvGetSize(tpl1);
	velocityX2 = cvCreateImage(size2, IPL_DEPTH_32F, 1);
	velocityY2 = cvCreateImage(size2, IPL_DEPTH_32F, 1);
	//cvConvertImage(tpl,curr1,0);
	
	printf("size2...%d\n",size2);
	printf("curr2...%d %d\n",curr2->width,curr2->height);
	cvCalcOpticalFlowLK(prev2, curr2, cvSize(3, 3),velocityX2, velocityY2);
	
	cvNamedWindow("Horizontal1", 1);
	cvShowImage("Horizontal1", velocityX2);

	cvNamedWindow("Vertical1", 1);
	cvShowImage("Vertical1", velocityY2);
	printf("curr width, height %d %d\n", curr2->width, curr2->height);
	vel1 = cvOpenFileStorage("second.txt",0,CV_STORAGE_APPEND);
for (int x=0; x<curr2->width; x=x+2) {
  for (int y=0; y<curr2->height; y=y+2) {
    double vel_x_here1 = cvGetReal2D( velocityX2, y, x);
    double vel_y_here1 = cvGetReal2D( velocityY2, y, x);
    cvLine( curr2, cvPoint(x, y), cvPoint(x+vel_x_here1 ,y+vel_y_here1), cvScalarAll(255));
	printf("%f , %f\n",vel_x_here1,vel_y_here1);
	
		cvWriteReal( vel1, "velx1",  vel_x_here1);
		cvWriteReal( vel1, "vely1", vel_y_here1 );
  }
}
	cvNamedWindow("result1", 1);
	cvShowImage("result1", curr2);

cvCopy(curr2,prev2,0);	
	/*CvScalar s;
		for (i=0;i<10;i++){
			for(j=0; j<10; j++){
		s=cvGet2D(tpl,i,j); // get the (i,j) pixel value
		printf("intensity=%f\n",s.val[0]);
			}
		}*/
	
//--------------
    cvMinMaxLoc( tm1, &minval1, &maxval1, &minloc1, &maxloc1, 0 );
	printf("minval,maxval...%d %d\n",minval1,maxval1);
    cvResetImageROI( frame );
    
	    /* if object found... */
    if( minval1 <= THRESHOLD ) {
        /* save object's current location */
        fullx2 = win_x02 + minloc1.x;
        fully2 = win_y02 + minloc1.y;
		printf("%d %d ...fullx2,fully2\n",fullx2,fully2);
		fs1 = cvOpenFileStorage("rectangle1.txt",0,CV_STORAGE_APPEND);
		cvWriteInt( fs1, "e5",fullx2 );
		cvWriteInt( fs1, "e6",fully2 );
 
        /* and draw a box there */
		printf("draw box");
		//-------------------
        cvRectangle( frame,
                     cvPoint( fullx2-5, fully2-5 ),
                     cvPoint( fullx2+5, fully2+5),
                     cvScalar( 0, 0, 255, 0 ), 1, 0, 0 );
		//-------------------
		 fruits = cvLoadImage( "degrees.jpg", 1);
		cvResizeWindow("fruits",1280,800);
			
		cvShowImage("fruits", fruits);	

		//cvResizeWindow("full2",400,400)f;
		//cvMoveWindow( "full2", 0, 0 );
//-------------------
		cvShowImage("full2", frame);
//-------------------
		cvWriteFrame(writer,frame); 
		printf("draw box over");
	
    } else {
        /* if not found... */
        fprintf( stdout, "Lost object.\n" );
        is_tracking = 0;
    }
		int obj_x01, obj_y01;

		obj_x01 = fullx2 - ( TPL_WIDTH  / 2 );
        obj_y01 = fully2 - ( TPL_HEIGHT / 2 ); 
        
        cvSetImageROI( frame, 
                       cvRect( obj_x01, 
                               obj_y01, 
                               TPL_WIDTH, 
                               TPL_HEIGHT ) );
        cvCopy( frame, tplnew1, NULL );
		cvConvertImage(tplnew1,curr2,0);
		cvNamedWindow("curr2", 1);
		cvShowImage("curr2", curr2);
		cvResetImageROI( frame );
		
		cvReleaseFileStorage( &fs );
		cvReleaseFileStorage( &fs1 );	
		cvReleaseFileStorage( &vel);
		cvReleaseFileStorage( &vel1 );	
		cvReleaseFileStorage( &eyefirst);
		cvReleaseFileStorage( &eyesecond );
}

