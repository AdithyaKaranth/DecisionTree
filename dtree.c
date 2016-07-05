/*
	Takes an input of n number of records with 3 attributes and a class.
	Does not work for every general case
*/	
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
struct NODE
{
	char att[4][10];
};
typedef struct NODE node;
struct ATT
{
	char attr[10][10];
	int count;
};
struct TREE
{
	char descript[10];
	int num,nodenum;
	struct TREE *llink,*rlink;
};
static int gName=0;
int num=1;
FILE *outputFile;
typedef struct TREE tree;
typedef struct ATT att;
int get_gini(node [],int,att []);
tree *decision_tree(node [],int, att[]);
tree *getnode();
void dotDump(tree *,FILE *);
void preorderDotDump(tree *,FILE *);
bool found(char [][10],int,char[]);
void set_attr(att [],int,int,node []);
void sort(int [],int);
void main()
{
	int n,i;
	FILE *pipe;
	att a[4];
	tree *root=NULL;
	scanf("%d",&n);
	node *input=(node *)malloc(n*sizeof(node));
	for(i=0;i<n;i++)
		scanf("%s%s%s%s",input[i].att[0],input[i].att[1],input[i].att[2],input[i].att[3]);
	for(i=0;i<4;i++)
	{
		a[i].count=0;
		set_attr(a,i,n,input);//to find the different cases in the attributes.
	}
	root=decision_tree(input,n,a);
	outputFile=fopen("dtree.dot","w");
	fclose(outputFile);
	outputFile=fopen("dtree.dot","a");
	if(outputFile!=NULL)
		dotDump(root,outputFile);
	fclose(outputFile);
	pipe=popen("dot -Tps dtree.dot -o dtree.ps","w");
	pclose(pipe);
	pipe=popen("evince dtree.ps","r");	
	pclose(pipe);
}
void preorderDotDump(tree *root,FILE *outputFile)
{
	if(root!=NULL)
	{
		fprintf(outputFile,"%d[label=%s",root->nodenum,root->descript);
		if(root->num>0&&root->num<4)
			fprintf(outputFile,"%d];\n",root->num);
		else
			fprintf(outputFile,"];\n");
		if(root->llink!=NULL)
			fprintf(outputFile,"%d -> %d ;\n",root->nodenum,root->llink->nodenum);
		if(root->rlink!=NULL)
			fprintf(outputFile,"%d -> %d;\n",root->nodenum,root->rlink->nodenum);
		preorderDotDump(root->llink,outputFile);
		preorderDotDump(root->rlink,outputFile);
	}
}			
void dotDump(tree *root,FILE *outputFile)
{
	gName++;
	fprintf(outputFile,"digraph BST {\n",gName);
	preorderDotDump(root,outputFile);
	fprintf(outputFile,"}\n");
}
tree *decision_tree(node input[],int n,att a[])
{
	int gini,c_count=0,b_count=0,i,j=0,flag=1;//to keep track of elements in array b and c
	node *b,*c;
	tree *t;
	char descript[10];
	for(i=1;i<n;i++)
		if(strcmp(input[i].att[3],input[i-1].att[3]))
		{
			flag=0;
			break;
		}
	if(flag||n==1)//leaf node
	{	
		t=getnode();
		strcpy(t->descript,input[0].att[3]);
		t->nodenum=num++;
		return t;
	}
		
	strcpy(descript,"attr");
	b=(node *)malloc(n*sizeof(node));
	c=(node *)malloc(n*sizeof(node));
	gini=get_gini(input,n,a);
	t=getnode();
	if(gini==3)
	{
		strcpy(t->descript,"ambiguous");
		t->nodenum=num++;
		return t;
	}
	strcpy(t->descript,descript);
	t->num=gini+1;
	t->nodenum=num++;
	for(i=0;i<n;i++)
	{
		if(!(strcmp(input[i].att[gini],a[gini].attr[0])))//assuming binary
		{
			for(j=0;j<4;j++)
				strcpy(b[b_count].att[j],input[i].att[j]);
			b_count++;
		}
		else//assuming binary
		{
			for(j=0;j<4;j++)
				strcpy(c[c_count].att[j],input[i].att[j]);
			c_count++;
		}
	}
	t->llink=decision_tree(b,b_count,a);
	t->rlink=decision_tree(c,c_count,a);
	return t;
}
tree *getnode()
{
	tree *x=(tree *)malloc(sizeof(tree));
	x->llink=x->rlink=NULL;
	x->num=0;
	return x;
}
int get_gini(node input[],int n,att a[])//returns the attribute the gives the least gini value
{
	int i,j,k=0,row,column,l,sum[10],table[10][10],*tmp,*class,ret_var=3,flag;//if ret_var=3,ambiguous case.
	float small=3,gini[10],temp;
	char temp1[10],temp2[10];
	row=a[3].count;
	for(i=0;i<3;i++)//GINI table
	{
		flag=0;
		column=a[i].count;
		for(j=0;j<row;j++)
		{
			strcpy(temp1,a[3].attr[j]);//class 
			for(k=0;k<column;k++)
			{
				table[j][k]=0;
				strcpy(temp2,a[i].attr[k]);
				for(l=0;l<n;l++)
					if(!(strcmp(temp2,input[l].att[i]))&&!(strcmp(temp1,input[l].att[3])))//equality of both attribute and class
						table[j][k]++;
			}			
			
		}
		memset(sum,0,sizeof(sum));
		for(l=0;l<column;l++)
		{
			for(j=0;j<row;j++)
				sum[l]+=table[j][l];
			if(sum[l]==0)//all the attributes in the records are same
			{
				flag=1;
				break;
			}
		}
		for(l=0;l<column;l++)
		{
			if(flag)//sum[l] can become 0
				break;
			gini[l]=1;
			for(j=0;j<row;j++)
				gini[l]-=((float)table[j][l]/sum[l])*((float)table[j][l]/sum[l]);
		}
		temp=0;
		for(l=0;l<column;l++)
		{
			if(flag)//sum[l] can become 0
			{
				temp=100;//small is initialised to 3.Can affect GINI value.
				break;
			}
			temp+=gini[l]*sum[l];
		}
		temp/=n;
		if(temp<small&&flag!=1)
		{
			ret_var=i;
			small=temp;
		}		
	}
	return ret_var;
}
void sort(int a[],int n)
{
	int i,j,temp;
	for(i=0;i<n-1;i++)
		for(j=0;j<n-i-1;j++)
			if(a[j]>a[j+1])
			{
				temp=a[j];
				a[j]=a[j+1];
				a[j+1]=temp;
			}
}
bool found(char attr[][10],int count,char input[])
{
	int i;
	for(i=0;i<count;i++)
		if(!(strcmp(attr[i],input)))
			return true;
	return false;
}
void set_attr(att a[],int i,int n,node input[])
{
	int j;
	strcpy(a[i].attr[a[i].count],input[0].att[i]);
	a[i].count++;
	for(j=1;j<n;j++)//the whole record
	{
		if(!(found(a[i].attr,a[i].count,input[j].att[i])))//to find the number of attributes to find GINI index
		{
			strcpy(a[i].attr[a[i].count],input[j].att[i]);
			(a[i].count)++;
		}
	}
}					
