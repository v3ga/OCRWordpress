#include "testApp.h"
#include <algorithm>

//--------------------------------------------------------------
void testApp::setup()
{
	wpBlogUserName = "admin";
	wpBlogUserPassword = "ioaccs7394";

	draggedRotationImageUINb = 0;

	mp_image = 0;
	mp_imageFiltered = 0;
	mp_teOcr = 0;
	mp_lblImageOriginalSize = 0;
	mp_lblImageFilteredSize = 0;
	

	imageFilterScale		= 2.5;
	imageFilterBlur			= 2;

	wpBlogPostId = -1;
	
	tesseract.setup();
	tesseract.setWhitelist("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,'?!-ˆŽ");
	tesseract.setAccuracy(ofxTesseract::ACCURATE);
	tesseract.setMode(ofxTesseract::AUTO);
	

	m_imageBlack.loadImage("imageBlack_400x400.png");
	
	//--------------------------------------------------------------
	//--------------------------------------------------------------
	gui = new ofxUISuperCanvas("Image originale");
	
	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	mp_image = gui->addImage("image", 0, 300, 300);


	gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	mp_lblImageOriginalSize = gui->addLabel("Original", "-", OFX_UI_FONT_SMALL);

	gui->addLabelButton("Rotation", false);

    gui->autoSizeToFitWidgets();
	gui->setHeight(800);
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);


	//--------------------------------------------------------------
	//--------------------------------------------------------------
	gui1 = new ofxUISuperCanvas("Filtre");
	gui1->setPosition(gui->getRect()->getWidth()+10,0);

	mp_imageFiltered = gui1->addImage("imageFiltered", 0, 300, 300);

	gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	mp_lblImageFilteredSize = gui1->addLabel("Original", "-", OFX_UI_FONT_SMALL);
	gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

	gui1->addLabel("OCR", "OCR", OFX_UI_FONT_SMALL);
    gui1->addSpacer();
	ofxUISlider* sliderScale = gui1->addSlider("scale", 1,4,imageFilterScale);
	ofxUISlider* sliderBlur = gui1->addSlider("blur", 1,3,imageFilterBlur);
	gui1->addLabelButton("Extraire", false);

	sliderScale->setTriggerType(OFX_UI_TRIGGER_END);
	sliderBlur->setTriggerType(OFX_UI_TRIGGER_END);

	gui1->addLabel("Resultat", "Resultat", OFX_UI_FONT_SMALL);
    gui1->addSpacer();

    gui1->autoSizeToFitWidgets();
	gui1->setHeight(800);

    mp_teOcr = gui1->addTextArea("imageText", "-",OFX_UI_FONT_SMALL);

    ofAddListener(gui1->newGUIEvent,this,&testApp::gui1Event);


	//--------------------------------------------------------------
	//--------------------------------------------------------------
	gui2 = new ofxUISuperCanvas("Wordpress");
	gui2->setWidth(340);
	gui2->setPosition(gui1->getRect()->x+gui1->getRect()->getWidth()+10,0);

	gui2->addLabel("Server", "Serveur", OFX_UI_FONT_SMALL);
    gui2->addSpacer();

	gui2->addTextInput("wpBlogHost", "localhost/", OFX_UI_FONT_SMALL);
	gui2->addTextInput("wpBlogHostPort", "80", OFX_UI_FONT_SMALL);
	gui2->addTextInput("wpBlogHostFile", "/DropboxClients/V3ga/lectures/Produit/", OFX_UI_FONT_SMALL);
	gui2->addTextInput("wpBlogHostFile2", "xmlrpc.php", OFX_UI_FONT_SMALL);

	gui2->addLabel("Post Id",OFX_UI_FONT_SMALL);
	gui2->addTextInput("wpBlogPostId", "", OFX_UI_FONT_SMALL);
	gui2->addLabelButton("Envoyer", false);
	gui2->addLabelButton("Editer en ligne", false);


	gui2->addLabel("Retour", "Retour", OFX_UI_FONT_SMALL);
    gui2->addSpacer();
    gui2->addTextArea("Feedback", "-",OFX_UI_FONT_SMALL);
	
	gui2->autoSizeToFitWidgets();
	gui2->setHeight(800);

    ofAddListener(gui2->newGUIEvent,this,&testApp::gui2Event);


	//--------------------------------------------------------------
	//--------------------------------------------------------------
	gui1->loadSettings(getSettingsDir("ocrSettings.xml"));
	gui2->loadSettings(getSettingsDir("wordpressSettings.xml"));

	wpBlogURL = "http://"+wpBlogHost+":"+ofToString(wpBlogHostPort)+wpBlogHostFile;
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	if (name == "Rotation")
	{
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
		if (button->getValue())
		{
			draggedRotationImageUINb++;
			rotateDraggedImage();
			filterDraggedImage(imageFilterScale,imageFilterBlur);

			updateUIDraggedImage();
			updateUIDraggedImageFiltered();
		}
	}
}

//--------------------------------------------------------------
void testApp::gui1Event(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	if (name == "Extraire")
	{
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
		if (button->getValue())
		{
			textOCR = runOcr();
			//textOCR.erase(std::remove_if(textOCR.begin(), textOCR.end(), '\n'), textOCR.end());
			textOCR.erase(std::remove(textOCR.begin(), textOCR.end(), '\n'), textOCR.end());
			

			printf("%s\n", textOCR.c_str());
			if (mp_teOcr){
				int nbCharsMax = 500;
			
				if (textOCR.size()>nbCharsMax)
					mp_teOcr->setTextString(textOCR.substr(0,nbCharsMax)+"...");
				else
					mp_teOcr->setTextString(textOCR);
			}
		}
	}
	else if (name == "scale")
	{
        ofxUISlider *slider = (ofxUISlider *) e.widget;
		imageFilterScale = slider->getValue();
		filterDraggedImage(imageFilterScale,imageFilterBlur);
		updateUIDraggedImageFiltered();
	}
	else if (name == "blur")
	{
        ofxUISlider *slider = (ofxUISlider *) e.widget;
		imageFilterBlur = slider->getValue();
		filterDraggedImage(imageFilterScale,imageFilterBlur);
		updateUIDraggedImageFiltered();
	}
}

//--------------------------------------------------------------
void testApp::gui2Event(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	if (name == "Envoyer")
	{
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
		if (button->getValue())
		{
			setWpPostBody();
		}
	}
	else if (name == "Editer en ligne")
	{
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
		if (button->getValue())
		{
			if (wpBlogPostId>0)
			{
				string command = "open "+getWpBlogURLAdminPostEdit(wpBlogPostId);
				system(command.c_str());
			}
		}
	}
	
// wpBlogURL
	
	else if (name == "wpBlogPostId")
	{
		ofxUITextInput* p = (ofxUITextInput*) e.widget;
		wpBlogPostId = ofToInt(p->getTextString());
	}
	else if (name == "wpBlogHost")
	{
		ofxUITextInput* p = (ofxUITextInput*) e.widget;
		wpBlogHost = p->getTextString();
	}
	else if (name == "wpBlogHostPort")
	{
		ofxUITextInput* p = (ofxUITextInput*) e.widget;
		wpBlogHostPort = ofToInt(p->getTextString());
	}
	else if (name == "wpBlogHostFile")
	{
		ofxUITextInput* p = (ofxUITextInput*) e.widget;
		wpBlogHostFile = p->getTextString();
	}
	else if (name == "wpBlogHostFile2")
	{
		ofxUITextInput* p = (ofxUITextInput*) e.widget;
		wpBlogHostFile2 = p->getTextString();
	}
}

//--------------------------------------------------------------
void testApp::rotateDraggedImage()
{
   draggedImage.rotate90(draggedRotationImageUINb);
   draggedImageResized = draggedImage;
}

//--------------------------------------------------------------
void testApp::filterDraggedImage(float scale, int medianSize)
{
	if (draggedImageResized.getWidth()>0)
	{
	  	draggedImageFiltered = draggedImageResized;
  		draggedImageFiltered.setImageType(OF_IMAGE_GRAYSCALE);
  		draggedImageFiltered.update();

  		draggedImageFiltered.resize(draggedImageResized.getWidth() * scale, draggedImageResized.getHeight() * scale);
  		medianBlur(draggedImageFiltered, medianSize);
	}
}

//--------------------------------------------------------------
void testApp::updateUIDraggedImage()
{
	if(draggedImage.getWidth()>0)
	{
		// Label
		if (mp_lblImageOriginalSize)
			mp_lblImageOriginalSize->setLabel( string(ofToString(draggedImage.getWidth()) + "x" + ofToString(draggedImage.getHeight()) + " px") );

		// Image UI
		draggedImageUI = draggedImage;
		
		ofRectangle imageRect( 0,0, draggedImage.getWidth(), draggedImage.getHeight() );
		imageRect.scaleTo( ofRectangle(0,0,mp_image->getRect()->getWidth(),mp_image->getRect()->getHeight()) );
		
		draggedImageUI.resize((int)imageRect.getWidth(), (int)imageRect.getHeight());
		if (mp_image){
			mp_image->setImage(&draggedImageUI);
		}
	}
}

//--------------------------------------------------------------
void testApp::updateUIDraggedImageFiltered()
{
	if (draggedImageFiltered.getWidth()>0)
	{
		// Label
		if (mp_lblImageFilteredSize)
			mp_lblImageFilteredSize->setLabel( string(ofToString(draggedImageFiltered.getWidth()) + "x" + ofToString(draggedImageFiltered.getHeight()) + " px") );

		// Image UI
		draggedImageFilteredUI = draggedImageFiltered;
		
		ofRectangle imageRect( 0,0, draggedImage.getWidth(), draggedImage.getHeight() );
		imageRect.scaleTo( ofRectangle(0,0,mp_image->getRect()->getWidth(),mp_image->getRect()->getHeight()) );
		
		draggedImageFilteredUI.resize((int)imageRect.getWidth(), (int)imageRect.getHeight());
		if (mp_imageFiltered){
			mp_imageFiltered->setImage(&draggedImageFilteredUI);
		}
	
	}
}


//--------------------------------------------------------------
string testApp::runOcr()
{
	if (draggedImageFiltered.getWidth()>0)
	{
		return tesseract.findText(draggedImageFiltered);
	}
	
	return "";
}

//--------------------------------------------------------------
// http://codex.wordpress.org/XML-RPC_WordPress_API/Posts#wp.newPost
// 
bool testApp::getWpPostBody(XmlRpcValue& result)
{
	if (wpBlogPostId>0)
	{
		XmlRpcClient c(wpBlogHost.c_str(), wpBlogHostPort, getWpBlogFile().c_str());
	  	XmlRpcValue args;

		args[0] 	= 1;
		args[1] 	= wpBlogUserName;
		args[2] 	= wpBlogUserPassword;
		args[3] 	= wpBlogPostId;

	 	if (c.execute("wp.getPost", args, result))
  		{
	    	std::cout << "result = " << result << std::endl;
//			std::cout << "post_content = "<< result["post_content"] << std::endl;

			return true;
  		}
	}
	
	return false;
}

//--------------------------------------------------------------
void testApp::setWpPostBody()
{
	ofLog() << " posting to " << wpBlogHost << ":" << wpBlogHostPort << getWpBlogFile() << "[postId=" << wpBlogPostId << "]";

	if (wpBlogPostId>0 && textOCR!="")
	{
		XmlRpcValue getResult;
		if ( getWpPostBody(getResult) )
		{
			string content = getResult["post_content"];
			content += "\n<p><blockquote>"+this->textOCR+"</blockquote></p>";

			XmlRpcClient c(wpBlogHost.c_str(), wpBlogHostPort, getWpBlogFile().c_str());
	  		XmlRpcValue args, result;
			XmlRpcValue argsContent;

			args[0] = 1;
			args[1] = "admin";
			args[2] = "ioaccs7394";
			args[3] = wpBlogPostId;
				argsContent["post_content"] = content;
			args[4] = argsContent;

		 	if (c.execute("wp.editPost", args, result))
  			{
	    		std::cout << "result = " << result << std::endl;
			 }
		}
	}
}

//--------------------------------------------------------------
void testApp::exit(){

    gui1->saveSettings(getSettingsDir("ocrSettings.xml"));
    gui2->saveSettings(getSettingsDir("wordpressSettings.xml"));
	delete gui;
	delete gui1;
	delete gui2;
}


//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
   ofBackground(50);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo info){


	if( info.files.size() == 1 )
	{
		textOCR = "";
		draggedRotationImageUINb=0;
		
		draggedImage.loadImage(info.files[0]);
		draggedImageResized = draggedImage;
		filterDraggedImage(2.5,2);

		updateUIDraggedImage();
		updateUIDraggedImageFiltered();
	}


}
