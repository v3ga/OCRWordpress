#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxTesseract.h"
#include "CvUtils.h"
#include "XmlRpc.h"

using namespace cv;
using namespace XmlRpc;

class testApp : public ofBaseApp{

	public:
		void setup();
	    void exit();
		void update();
		void draw();
	
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// Settings
		string					getSettingsDir(){return "settings/gui/";}
		string					getSettingsDir(string file){return (getSettingsDir()+file);};

		// OCR
		ofxTesseract 			tesseract;
		string					textOCR;
		string  				runOcr();

		// Images
		int 					draggedRotationImageUINb;
		ofImage					draggedImage, draggedImageResized, draggedImageUI;
		void 					updateUIDraggedImage();
		void					rotateDraggedImage();


		// Filters
		ofImage					draggedImageFiltered, draggedImageFilteredUI;
		float					imageFilterScale;
		float					imageFilterBlur;
		void					filterDraggedImage(float scale, int medianSize);
		void 					updateUIDraggedImageFiltered();


		// Wordpress
		string					wpBlogUserName;
		string					wpBlogUserPassword;
		int						wpBlogPostId;
		string					wpBlogHost, wpBlogHostFile, wpBlogHostFile2, wpBlogURL;
		int						wpBlogHostPort;
		string					getWpBlogFile(){return wpBlogHostFile+wpBlogHostFile2;}
		string					getWpBlogURLAdmin(){return (wpBlogURL + "wp-admin/");}
		string					getWpBlogURLAdminPostEdit(int postId){return (getWpBlogURLAdmin()+"post.php?post="+ofToString(postId)+"&action=edit");}
	
		bool					getWpPostBody	(XmlRpcValue& result);
		void 					setWpPostBody	();

		// UI
		ofxUISuperCanvas		*gui,*gui1,*gui2;
		ofImage 				m_imageBlack;

		ofxUIImage				*mp_image,*mp_imageFiltered;
		ofxUITextArea*			mp_teOcr;
		ofxUILabel				*mp_lblImageOriginalSize, *mp_lblImageFilteredSize;

		void 					gui1Event(ofxUIEventArgs &e);
		void 					guiEvent(ofxUIEventArgs &e);
		void 					gui2Event(ofxUIEventArgs &e);
};
