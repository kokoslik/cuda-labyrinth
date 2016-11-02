#include <iostream>
#include <utility>
#include <set>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    Mat frame,grayframe;
    frame=imread("../Labyrinth_1h.png");
    cvtColor(frame,grayframe,CV_BGR2GRAY);
    //threshold(grayframe,grayframe,254,255,CV_THRESH_BINARY);
    namedWindow("Window");
    int bsize=4,wallsize=7,corsize=93;
    uchar start=150,stop=76,good=29;
    Mat lab((grayframe.rows-2*bsize)/(corsize+wallsize)*2+3,(grayframe.cols-2*bsize)/(corsize+wallsize)*2+3,CV_8UC1,Scalar(255));
    for(int i=1;i<lab.rows-1;i++)
    {
        uchar *ptr=lab.ptr<uchar>(i);
        uchar *ptr2=grayframe.ptr<uchar>(bsize+(i/2)*corsize+((i-1)/2)*wallsize+((i+1)%2)*wallsize/2+(i%2)*corsize/2);
        for(int j=1;j<lab.cols-1;j++)
            ptr[j]=ptr2[bsize+(j/2)*corsize+((j-1)/2)*wallsize+((j+1)%2)*wallsize/2+(j%2)*corsize/2];
    }


    uchar *ptr=lab.data;
    //Печать лабиринта в консоль
//    for(int i=0;i<lab.rows;i++)
//    {
//        for(int j=0;j<lab.cols;j++)
//            cout<<(int)ptr[i*lab.cols+j]<<'\t';
//    cout<<endl;
//    }
//    cin.get();
    set<int> opened,closed;
    map< int, int > parents,hfunc;
    int startidx=(int)(find(ptr,ptr+lab.rows*lab.cols,start)-ptr);
    int stopidx=(int)(find(ptr,ptr+lab.rows*lab.cols,stop)-ptr);
    opened.insert(startidx);
    parents[startidx]=-1;
    hfunc[startidx]=0;
    bool flag=true;
    while(!opened.empty() && flag)
    {
        int minf=hfunc[*opened.begin()]+10*abs(stopidx/lab.cols-(*opened.begin())/lab.cols)+10*abs(stopidx%lab.cols-(*opened.begin())%lab.cols);
        set<int>::iterator minfiter=opened.begin();
        for(set<int>::iterator i=opened.begin();i!=opened.end();i++)
                if(hfunc[*i]+10*abs(stopidx/lab.cols-(*i)/lab.cols)+10*abs(stopidx%lab.cols-(*i)%lab.cols)<minf)
                {
                    minf=hfunc[*i]+10*abs(stopidx/lab.cols-(*i)/lab.cols)+10*abs(stopidx%lab.cols-(*i)%lab.cols);
                    minfiter=i;
                }
        int cur=*minfiter;
        opened.erase(minfiter);
        closed.insert(cur);
        for(int i=-1;i<2;i++)
            for(int j=-1;j<2;j++)
            {
                if(((i!=0)||(j!=0))&&(i*j==0)&&(ptr[cur+i*lab.cols+j]!=255)&&(closed.count(cur+i*lab.cols+j)==0))
                      if(opened.insert(cur+i*lab.cols+j).second)
                      {
                          int weight=ptr[cur+i*lab.cols+j]==good?-10:10;
                          parents[cur+i*lab.cols+j]=cur;
                          hfunc[cur+i*lab.cols+j]=weight+hfunc[cur];
                      }
                      else
                      {
                          int weight=ptr[cur+i*lab.cols+j]==good?-10:10;
                          if(hfunc[cur+i*lab.cols+j]>weight+hfunc[cur])
                          {
                              parents[cur+i*lab.cols+j]=cur;
                              hfunc[cur+i*lab.cols+j]=weight+hfunc[cur];
                          }

                      }
                if(cur+i*lab.cols+j==stopidx)
                    flag=false;
            }
        for(set<int>::iterator it=opened.begin();it!=opened.end();it++)
        {
            int i=(*it)/lab.cols,j=((*it)%lab.cols);
            circle(frame,Point2i(bsize+(j/2)*corsize+((j-1)/2)*wallsize+((j+1)%2)*wallsize/2+(j%2)*corsize/2,bsize+(i/2)*corsize+((i-1)/2)*wallsize+((i+1)%2)*wallsize/2+(i%2)*corsize/2),5,Scalar(255,255,0),4);
        }
        for(set<int>::iterator it=closed.begin();it!=closed.end();it++)
        {
            int i=(*it)/lab.cols,j=((*it)%lab.cols);
            circle(frame,Point2i(bsize+(j/2)*corsize+((j-1)/2)*wallsize+((j+1)%2)*wallsize/2+(j%2)*corsize/2,bsize+(i/2)*corsize+((i-1)/2)*wallsize+((i+1)%2)*wallsize/2+(i%2)*corsize/2),5,Scalar(0,255,255),4);
        }
        imshow("Window",frame);
        waitKey(60);
    }

    int id=stopidx;
    vector<Point2i> centers;
    while(id!=-1)
    {
        centers.push_back(Point2i(bsize+(id%lab.cols-1)/2*(corsize+wallsize)+corsize/2,bsize+(id/lab.cols-1)/2*(corsize+wallsize)+corsize/2));
        id=parents[id];
    }
    for(int i=0;i<centers.size()-1;i++)
        line(frame,centers[i],centers[i+1],Scalar(255,0,0),2);

    imshow("Window",frame);
    waitKey(0);
    return 0;
}
