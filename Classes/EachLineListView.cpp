//
//  EachLineListView.cpp
//  CSVConfig
//
//  Created by xiayangqun on 16/6/22.
//
//

#include "EachLineListView.h"
#include "HelloWorldScene.h"

EachLineListVew * EachLineListVew::createWithFileName(const std::string& fileName)
{
    auto ptr=new EachLineListVew();
    if(ptr&&ptr->initWithFileName(fileName))
    {
        ptr->autorelease();
        return ptr;
    }
    
    CC_SAFE_DELETE(ptr);
    return nullptr;
    
}


bool EachLineListVew::initWithFileName(const std::string& fileName)
{
    if(!Node::init())
        return false;
    
    listView=ListView::create();
    listView->setContentSize(Size(Director::getInstance()->getVisibleSize().width, Director::getInstance()->getVisibleSize().height-150));
    listView->setAnchorPoint(Vec2::ZERO);
    listView->setPosition(Vec2(0,100));
    listView->setBounceEnabled(true);
    addChild(listView);
    

    if(fileName!="")
    {
        if(FileUtils::getInstance()->isFileExist(fileName))
        {
            CCSVParse pr;
            pr.openFile(fileName.c_str());
            
            for(int i=0;i<pr.data.size();i++)
            {
                auto lineDate=pr.data[i];
                lineDate.erase(lineDate.begin());
                auto item=EachLineWiget::createWithButtonValue(lineDate, i+1);
                item->onUpOrDeleteCall=CC_CALLBACK_2(EachLineListVew::onUpOrDelete,this);
                listView->pushBackCustomItem(item);
            }
            
        }
        else
        {
            MessageBox("在指定目录下没有找到需要修改的CSV文件", "出错了");
        }
    }

    
    auto button=ui::Button::create("button.png");
    button->setTitleFontSize(30);
    button->setTitleColor(Color3B::YELLOW);
    button->setScale9Enabled(true);
    button->setContentSize(Size(200,50));
    button->setTitleText("增加新行");
    button->setPositionX(120);
    button->setPositionY(60);
    button->addClickEventListener(CC_CALLBACK_1(EachLineListVew::onButtonClickToAddLine, this));
    addChild(button);
    
    
    fileNameEditBox=ui::EditBox::create(Size(400,50), Scale9Sprite::create("button.png"));
    fileNameEditBox->setPosition(Vec2(500,60));
    fileNameEditBox->setPlaceHolder("在这里输入保存的文件名字");
    addChild(fileNameEditBox);
    
    
    auto button2=ui::Button::create("button.png");
    button2->setTitleFontSize(30);
    button2->setTitleFontSize(30);
    button2->setScale9Enabled(true);
    button2->setContentSize(Size(200,50));
    button2->setTitleText("保存文件");
    button2->setPositionX(800);
    button2->setPositionY(60);
    button2->addClickEventListener(CC_CALLBACK_1(EachLineListVew::onButtonClickToSaveCSVFile, this));
    addChild(button2);
    
    
    auto button3=ui::Button::create("button.png");
    button3->setTitleFontSize(20);
    button3->setTitleColor(Color3B::RED);
    button3->setScale9Enabled(true);
    button3->setContentSize(Size(280,50));
    button3->setTitleText("返回(别忘记了保存你的文件)");
    button3->setPositionX(1100);
    button3->setPositionY(60);
    button3->addClickEventListener([](Ref * sender){
        Director::getInstance()->replaceScene(HelloWorld::createScene());
    });
    addChild(button3);
    
    
    return true;
}


void  EachLineListVew::onButtonClickToAddLine(Ref * sender)
{

    std::string  str ("");
    auto buttonTitleNames=CSVGlobalConfig::getInstance()->getLineOptions(str);
    
    auto line=EachLineWiget::createWithButtonTitle(buttonTitleNames, listView->getItems().size()+1);
    line->onUpOrDeleteCall=CC_CALLBACK_2(EachLineListVew::onUpOrDelete,this);
    listView->pushBackCustomItem(line);

}

void EachLineListVew::onButtonClickToSaveCSVFile(Ref * sender)
{
    if( strcmp( fileNameEditBox->getText(),"")==0)
    {
        MessageBox("请先设置好保存的CSV文件的名字", "提示");
    }
    else
    {
        auto writeString=getCSVString();
        auto fullPath=FileUtils::getInstance()->getWritablePath()+ fileNameEditBox->getText();
        FileUtils::getInstance()->writeStringToFile(writeString, fullPath);
        MessageBox(fullPath.c_str(), "文件已经保存");
    }
}



//1 是删除，2是向上移动
void EachLineListVew::onUpOrDelete(EachLineWiget * sender,int tag)
{
    ssize_t index=listView->getIndex(sender);
    if(tag==1)
        listView->removeItem(index);
    else if(tag==2&&index>0)
    {
        sender->retain();
        listView->removeItem(index);
        listView->insertCustomItem(sender,index-1);
        sender->retain();
    }
    else if(tag==3&&index<listView->getItems().size()-1)
    {
        sender->retain();
        listView->removeItem(index);
        listView->insertCustomItem(sender,index+1);
        sender->retain();
    }
    
    changeItemsIndex();

}

void EachLineListVew::changeItemsIndex()
{
    auto& items=listView->getItems();
    for(int i=0;i<items.size();i++)
    {
        auto item=items.at(i);
        ((EachLineWiget *)item)->changIndex(i+1);
    }

}


std::string EachLineListVew::getCSVString()
{
    EachLineWiget * item=nullptr;
    auto items=listView->getItems();
    string retStr;
    
    for(auto& widgetItem: items)
    {
        item=dynamic_cast<EachLineWiget *>(widgetItem);
        retStr=retStr+item->getLineOptionString();
    }
    
    return retStr;
}