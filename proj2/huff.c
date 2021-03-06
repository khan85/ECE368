#include "huff.h"
#include <string.h>


int count_Freq(char * filename, ASCII_Freq * frequencies)
{
  //Variables
  FILE *fptr=fopen(filename, "r"); //Opens file in read mode
  int count;

  //Calculations
  if (fptr==NULL) //If the file is NULL it terminates
    {
      return 0;
    }
  count=0; //Setting counter to zero
  while(!feof(fptr)) //Used to cycle through until it reaches EOF
    {
      int ochar=fgetc(fptr);
      if(ochar!=EOF)
	{
	  count++;
	  frequencies[ochar].value=(char)ochar;
	  frequencies[ochar].frequency++;
	}
    }
  fclose(fptr);
  return 1;
}

/*void destroy_tree(TreeNode *tn)
{
  if(tn==NULL)
    {
      return;
    }
  destroy_tree(tn->left);
  destroy_tree(tn->right);
  free(tn);
  }*/

static int compare_freq(const void *p1, const void *p2)
{
  //Variables
  const ASCII_Freq * ind_point1 = (const ASCII_Freq *)p1; //Standard typecasting technique used here
  const ASCII_Freq * ind_point2 = (const ASCII_Freq *)p2;
  const int ind_value1 = ind_point1->frequency;
  const int ind_value2 = ind_point2->frequency;
  //Calculations
  return (ind_value1-ind_value2); //Returns the difference of the values
}

void sort_freq(ASCII_Freq *frequencies)
{
  qsort(frequencies, 128, sizeof(ASCII_Freq), compare_freq); //Sorts the frequencies
}

//Tree Node functions below:

TreeNode *create_TreeNode(char val, int freq)
{
  TreeNode *tree_node= malloc(sizeof(TreeNode)); //Creates the TreeNode
  tree_node->left=NULL;
  tree_node->right=NULL;
  tree_node->val=val;
  tree_node->frequency=freq;
  return tree_node;
}

TreeNode *merge_TreeNode(TreeNode *tn_1, TreeNode *tn_2)
{
  TreeNode *tree_node=malloc(sizeof(TreeNode)); //Merges TreeNode in standard way
  tree_node->left=tn_1;
  tree_node->right=tn_2;
  tree_node->val=0;
  tree_node->frequency= tn_1->frequency + tn_2->frequency;
  return tree_node;
}

//ListNode functions below
ListNode *create_list(TreeNode *tree_node)
{
  ListNode *list_node= malloc(sizeof(ListNode)); //Creates a ListNodein a standard way
  list_node->next=NULL;
  list_node->tptr=tree_node;
  return list_node;


}


ListNode *insert_list(ListNode *head, ListNode *n)
{
  //Variables
  int freq_1;
  int freq_2;
  
  //Calculations
  if(head==NULL) //If list is empty then the new ListNode n is the list
    {
      return n;
    }
  
  if(n==NULL) //Returns error if n is empty
    {
      printf("Error in insert_list!\n");
    }

  if((n->next)!=NULL) //If it equals NULL then you can add n at the end of the first list
    {
      printf("error!\n");
    }

  freq_1=(head->tptr)->frequency; //Assigning frequencies
  freq_2=(n->tptr)->frequency;
  if(freq_1 > freq_2) //if freq_1 is more than freq_2 then we need to add the old list after n
    {
      n->next=head;
      return n;
    }
  
  head->next= insert_list(head->next,n); //Recursive call to get to continue function
  return head;
}

ListNode *build_list(ASCII_Freq * frequencies)
{
  //Variables
  int i=0;
  ListNode * h = NULL;

  //Executions
  while(frequencies[i].frequency==0)//Used to find the first frequency which isn't zero
    {
      i++;
    }
  if(i == 128) //Should not be more than 128; no letter
    {
      return NULL;
    }

  while(i<128)//Runs while letters appears and makes a list with it
    {
      TreeNode *tree_node= create_TreeNode(frequencies[i].value, frequencies[i].frequency);
      ListNode *list_node= create_list(tree_node);
      h = insert_list(h,list_node); //Each node points to a tree
      i++;
    }
  return h;
}



int theight_help(TreeNode *tree_node, int height) //This function traverses through the tree and terminates when it reaches NULL. The height is returned with the help of a counter
{
  if(tree_node==0) //Base case for recursive call
    {
      return height;
    }
  int left= theight_help(tree_node->left,height+1); //Traverses towars the left side of the tree
  int right= theight_help(tree_node->right, height+1);//Traverses to the right
  
  if(left>right)
    {
      return left;
    }
  if(right>left)
    {
      return right;
    }
  return left;
}

void tleaf_help(TreeNode *tree_node, int *number)//Used to count the number of leaf nodes of the tree
{
  if(tree_node==0) //Base case for recursive call to terminate on
    {
      return;
    }
  TreeNode *left= tree_node->left; 
  TreeNode *right= tree_node->right;

  if((right==NULL)&&(left==NULL)) //If both left and right are NULL that means the node must be a leaf node
    {
      (*number)++; //Keep track of number of leaf nodes
      return;
    }
  //To traverse through the tree
  tleaf_help(left,number);
  tleaf_help(right, number);
}

int theight(TreeNode *tree_node) //calls the helper function which calculates the height
{
  return theight_help(tree_node,0);

}
      
int tleaf(TreeNode *tree_node) //call the helper function which calculates the number of leaf nodes
{
  int number=0;
  tleaf_help(tree_node, &number);
  return number;
}


void build_cbookhelp(TreeNode *tree_node, int * * cbook, int *row, int col)
{
  if(tree_node==NULL)
    {
      return;
    }
  TreeNode *left= tree_node->left;
  TreeNode *right= tree_node->right;
  
  if((right==NULL)&&(left==NULL)) //To see if it is a leaf node
    {
      cbook[*row][0]=tree_node->val; //To build the actual codebook to be used
      (*row)++;
      return;
    }
  if(left!=NULL) //If the left side is NULL it adds characters in that column of the codebook 
    {
      int nrow= tleaf(left);
      int i=0;
      for(i= *row; i<(*row)+nrow;i++) //Starts adding code to the specific column of the codebook
	{
	  cbook[i][col]=0;
	}
      build_cbookhelp(left,cbook,row,col+1); //Recursive call to go through the tree
    }

  if(right!=NULL) //Same as above but for adding code to the rows in the codebook instead of column
    {
      int nrow= tleaf(right);
      int i=0;
      for(i=*row; i<(*row)+nrow;i++)
	{
	  cbook[i][col]=1;
	}
      build_cbookhelp(right,cbook,row,col+1);
    }
}

void build_cbook(TreeNode *root, int ** cbook)//Calls helper function
{
  int row =0;
  build_cbookhelp(root, cbook, &row,1);
}

int wBit(FILE *fptr, unsigned char bit, unsigned char *wbit, unsigned char *cbyte)//Used to write one bit to a file. It goes from 0 to 7 and when wbit gets to 7 its restarts as thats one byte.
{
  if((*wbit)==0) //wbit refers to the bit on which its at
    {
      *cbyte=0;
    }
  //Bit shiftings done below
  unsigned char temporary=bit<< (7-(*wbit));
  *cbyte |= temporary; //The current byte stores the temporary value. The temporary changes through the course of the function
  int value;
  value=0;

  if((*wbit)==7) //Once it gets to 7 a byte is written to the file
    {
      int r;
      r=fwrite(cbyte,sizeof(unsigned char),1,fptr);
      if(r==1)
	{
	  value=1; //Returns 1 if its successful
	}
      else
	{
	  value=-1; //Returns -1 if it fails
	}
    }
  *wbit= ((*wbit)+1)%8;
  return value;
}

static void char2bits(FILE *optr, int ch, unsigned char *wbit, unsigned char *cbyte)//To print the bits of ASCII characters
{
  unsigned char masking=0x40; //Sets it to 7 bits only
  while(masking >0)
    {
      wBit(optr, (ch&masking)==masking,wbit,cbyte);
      masking >>=1;
    }
}

static void thead_help(TreeNode *tree_node, FILE *optr, unsigned char *wbit, unsigned char *cbyte)
{
  if(tree_node==NULL) //Terminating statements
    {
      return;
    }
  TreeNode *left= tree_node->left;
  TreeNode *right= tree_node->right;
  if((right==NULL)&&(left==NULL)) //If its a leaf node write the bit and print it
    {
      wBit(optr, 1, wbit, cbyte);
      char2bits(optr,tree_node->val,wbit,cbyte);
      return;
    }
  thead_help(left,optr,wbit,cbyte); //recursively calls the left side of the tree
  thead_help(right,optr,wbit,cbyte); //recursively call the right side of the tree
  wBit(optr,0,wbit,cbyte);
}

void theaders(TreeNode *tree_node, char *out) //Used to call the header helper function
{
  FILE *optr=fopen(out,"w"); //opens the file in write mode and assigns it to optr
  if(optr==NULL)//Terminate code if it comes here
    {
      return;
    }
  unsigned char whbit=0; //to keep track of the bit position
  unsigned char cbyte=0;//To keep track of the current byte
  thead_help(tree_node,optr,&whbit,&cbyte); 
  while(whbit!=0) 
    {
      wBit(optr,0,&whbit,&cbyte); //To write bits at the end of the current byte if there are unused bits
    }
  unsigned char nline='\n';
  fwrite(&nline,sizeof(unsigned char),1,optr);
  fclose(optr);
}
//Finally the compress function
int compress(char * infile, char *outfile, int ** cbook, int * mapping)
{
  FILE *iptr=fopen(infile,"r"); //opens the input file in read mode
  if(iptr==NULL) //If null the function terminates
    {
      return 0;
    }
  FILE *optr=fopen(outfile,"a"); //It will append the output file and not overwrite
  if(optr==NULL) //If null terminates
    {
      fclose(optr);
      return 0;
    }
  unsigned char wbit=0;//To keep track of the bit you're on
  unsigned char cbyte=0;//To keep track of the current byte
  while(!feof(iptr)) //To go through the file
    {
      int ochar=fgetc(iptr);
      if(ochar!=EOF)
	{
	  int i=mapping[ochar]; 
	  int j=1;
	  while(cbook[i][j]!=-1) //-1 is where it would terminate
	    {
	      wBit(optr,(cbook[i][j]==1),&wbit,&cbyte);//Writes bit by bit
	      j++; 
	    }
	}
    }
  while(wbit!=0) //Last bits are zero
    {
      wBit(optr,0,&wbit,&cbyte);
    }
  fclose(iptr);
  fclose(optr);
  return 1;
}

int **encode_list(char *in_file,char *out_file)
{
  //Variables
  int k;
  ASCII_Freq *text_freq= (ASCII_Freq *)calloc(128, sizeof(ASCII_Freq)); //Used calloc instead of malloc as it initializes values to zero. Was causing faults for some reason with malloc

  //Executions
  if(count_Freq(in_file,text_freq)==0) //Terminates if these are zero
      {
        return 0;
      }
    
  sort_freq(text_freq); //Sorts them with qsort

  k=0; //Prints the frequencies
  for(k=0;k<128;k++)
    {
      printf("%c\t%d\n", text_freq[k].value, text_freq[k].frequency);               }
  ListNode *head=build_list(text_freq);

  if(head==NULL)//Terminates if header is NULL
    {
      return 0;
    }

  while(head->next!=NULL ) //Keeps going until it reaches end of list
    {
      ListNode *sec=head->next;//Assigns the second and third item in the list to ListNode pointers 
      ListNode *thr=sec->next;

      TreeNode *left=head->tptr;
      TreeNode *right=sec->tptr;
      
      free(head); //Free the assigned nodes
      free(sec);
	
      head=thr; 
	
      TreeNode *r_mrg=merge_TreeNode(left, right); //Merges the two sides of the tree together
      ListNode *current=create_list(r_mrg);//Creates a list
      head=insert_list(head,current);//The header is inserted into the list at the end
    }

  TreeNode *root=head->tptr;
  free(head);
  int row=tleaf(root); //To get rows and columns
  int column=theight(root);
  column=column+1;
  int **codebook=(int **)calloc(row, sizeof(int *));//Calloc used again as malloc led to errors
  int i=0;
  int j=0;
  for(i=0;i<row;i++) //Encode the rwos and columns 
    {
      codebook[i]=(int *)calloc(column,sizeof(int));
      for(j=0;j<column;j++)
	codebook[i][j] = -1;
    }
  
  build_cbook(root,codebook);//To build the codebook with the given root
  int mapping[128];
  for(i=0;i<128;i++) //Bit mapping done below
    {
      mapping[i]=-1;
      for(j=0;j<row;j++)
        {
	  if(codebook[j][0]==i)
            {
	      mapping[i]=j;
            }
        }
    }

  compress(in_file,out_file,codebook,mapping); //compresses everything


  return codebook;//Returns codebook 
}

int main(int argc, const char **argv)
{
  char *fn= malloc(sizeof(char));
  char *out=NULL;
  printf("Enter input file:");
  scanf("%s",fn);
  out=(char *)calloc(strlen(fn)+7,sizeof(char));
  strcat(out,fn);
  strcat(out,".huff");
  encode_list(fn,out);
  return 0;

}
