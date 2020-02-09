#include	<algorithm>
#include	<iostream>
#include	<fstream>
#include	<cfloat>
#include	<vector>
#include	<cmath>
using	namespace	std;
const	size_t	forcast=100;
struct	Data{	double	x,	y;	};
vector<Data>	data;

bool	load_data(const	char	*F){
	ifstream	fi(F);
	if(!fi)	return	false;
	string	s;	fi>>s>>s;	Data	d;
	for(fi>>d.x>>d.y;	!fi.eof();	fi>>d.x>>d.y)	data.push_back(d);
	fi.close();
	cerr<<F<<'\t'<<data.begin()->x<<'\t'<<data.rbegin()->x<<'\n';
	return	true;
}

void	mcmc(Data	*d,	size_t	n,	double	*pre,	bool	F){
	vector<double>	v(n-2);
	for(size_t	i=0;	i<v.size();	i++)	v[i]=log(log(d[i+2].y/d[i+1].y)/log(d[i+1].y/d[i].y));
	sort(v.begin(),v.end());
	double	lambda=v.size()%2?v[v.size()/2]:0.5*(v[v.size()/2]+v[v.size()/2-1]);
	double	y=d[n-1].y,	z=d[n-2].y;
	for(size_t	i=0;	i<forcast;	i++){	pre[i]=exp(exp(lambda)*log(y/z))*y;	z=y;	y=pre[i];	}
}

int	main(void){
	if(!load_data("data.txt"))	return	0;
	vector<float>	alpha;
	ofstream	fo("alpha.txt");
	fo<<"day\talpha_i\talpha_hat\n";
	for(size_t	i=2;	i<data.size();	i++){
		float	a=log(data[i].y/data[i-1].y)/log(data[i-1].y/data[i-2].y);
		alpha.push_back(a);
		vector<float>	t=alpha;	sort(t.begin(),t.end());
		float	b=t.size()%2?t[t.size()/2]:0.5*(t[t.size()/2]+t[t.size()/2-1]);
		fo<<data[i].x<<'\t'<<log(data[i].y/data[i-1].y)/log(data[i-1].y/data[i-2].y)<<'\t'<<b<<'\n';;
	}
	fo.close();
	double	loss=0,	n=0;
	cout<<"day\tWHO\tpredict\terror%\n";
	for(size_t	i=0;	i<=data.size();	i++){
		if(data[i].x<=-3){	 cout.precision(0);  cout.setf(ios::fixed);	cout<<data[i].x<<'\t'<<data[i].y<<'\n';	}
		else{
			double	pre[forcast];
			mcmc(data.data(),	i,	pre,	i==data.size());
			if(i<data.size()){
				cout.precision(0);	cout.setf(ios::fixed);	cout<<data[i].x<<'\t'<<data[i].y<<'\t'<<pre[0]<<'\t';
				cout.precision(2);	cout.setf(ios::fixed);	cout<<(pre[0]-data[i].y)/data[i].y*100<<"%"<<'\n';
				loss+=fabs((pre[0]-data[i].y)/data[i].y);	n+=1;
			}else{
				for(size_t	j=0;	j<forcast;	j++){	cout.precision(0);  cout.setf(ios::fixed);	cout<<data.rbegin()->x+j+1<<'\t'<<'\t'<<pre[j]<<'\n';	}
			}
		}
	}
	cerr<<"loss\t"<<100*loss/n<<"%\n";
	return	0;
}
