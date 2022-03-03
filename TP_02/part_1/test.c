int main(){
  int i,j,out = 0,k;
  double hello;
  for(i=0;i<10;i++)
    for(j=0;j<10;j++)
      out += i+j;
  //return out; for some reason, the return-type warning is not thrown when the warning is active...
}
