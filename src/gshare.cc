#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#define TAKEN     ((const unsigned char)'t')
#define NOT_TAKEN     ((const unsigned char)'n')

int main(int argc, char *argv[])

	{
		FILE *trace_file;
		int cursor=0;
		long line=1;
		char *branch_prediction_type=NULL;
		int m1_parameter=0, m2_parameter=0, n_parameter=0, k_parameter=0;
		int btb_size=0, btb_assoc=0;
		char status;
		char pc_add[9] = "";
		char pc_add_temp_nibble[4] = "";
		int i=0, base=1;
		int correct_predictions=0, incorrect_predictions=0;
		int branch_index_decimal=0, branch_index_decimal_div_10=0;
		int pc_add_binary_offset_for_m2=0;
    int pc_add_binary_offset_for_m1_minus_n=0;
    int pc_add_binary_offset_for_n=0;

		char pc_add_binary[33] = "";
		char branch_index_binary[33] = "";
    char branch_index_binary_excluding_n[33] = "";
    char branch_history_binary[33] = "";

		char one_zero[9] = "0";
    char two_zero[9] = "00";
    char three_zero[9] = "000";
    char four_zero[9] = "0000";
    char five_zero[9] = "00000";
    char six_zero[9] = "000000";
    char seven_zero[9] = "0000000";

		int *prediction_table = NULL;
    char (*global_branch_history_register) = NULL;
    int *xor_result=NULL;
    //char zero = "0";

		branch_prediction_type = argv[1];
		//only assign if branch_prediction_type is bimodal
    if(strcmp(branch_prediction_type,"bimodal")==0)
    {
      //printf("Inside bimodal\n");
      m2_parameter = atoi(argv[2]);
  		btb_size = atoi(argv[3]);
  		btb_assoc = atoi(argv[4]);
  		trace_file = fopen(argv[5],"r");
  		prediction_table = (int*)malloc(sizeof(int)*(pow(2,m2_parameter)));
  		//printf("Size of prediction_table: %d\n", sizeof(prediction_table));
  		for(i=0;i<(pow(2,m2_parameter));i++)
  		{
  			prediction_table[i] = 2;
  			//printf("prediction_table value at %d: %d\n",i, prediction_table[i]);
  		}
    }

		//only assign if branch_prediction_type is gshare
    if(strcmp(branch_prediction_type,"gshare")==0)
    {
      //printf("Inside gshare\n");
      m1_parameter = atoi(argv[2]);
      n_parameter = atoi(argv[3]);
      btb_size = atoi(argv[4]);
      btb_assoc = atoi(argv[5]);
      trace_file = fopen(argv[6], "r");
      prediction_table = (int*)malloc(sizeof(int)*(pow(2,m1_parameter)));
      for(i=0;i<(pow(2,m1_parameter));i++)
  		{
  			prediction_table[i] = 2;
  			//printf("prediction_table value at %d: %d\n",i, prediction_table[i]);
  		}
      global_branch_history_register = (char(*))malloc(sizeof(char)*n_parameter);
      for(i=0;i<n_parameter;i++)
      {
        global_branch_history_register[i]='0';
      }
      global_branch_history_register[n_parameter] = '\0';

      //printf("Inital value of GBHR and size and strlen: %s %d %d\n", global_branch_history_register,sizeof(global_branch_history_register), strlen(global_branch_history_register));
      xor_result = (int(*))malloc(sizeof(int)*n_parameter);
      for(i=0;i<n_parameter;i++)
      {
        xor_result[i]=0;
      }
      //xor_result[n_parameter] = \0;
    }

		//only assign if branch_prediction_type is hybrid

		fseek(trace_file, 0, SEEK_SET);

    while ((cursor=fgetc(trace_file)) != EOF)
        {
            fseek(trace_file, -1, SEEK_CUR);
            fscanf(trace_file, "%s %c\n", &pc_add, &status);
            //fscanf(trace_file, "%c", &status);
						//printf("String length: %d\n", strlen(pc_add));

						if(strlen(pc_add)==7)
                {
                    strcat(one_zero,pc_add);
                    strcpy(pc_add,one_zero);
                    strcpy(one_zero,"0");
                }
            else if(strlen(pc_add)==6)
                {
                    strcat(two_zero,pc_add);
                    strcpy(pc_add,two_zero);
                    strcpy(two_zero,"00");
                }
            else if(strlen(pc_add)==5)
                {
                    strcat(three_zero,pc_add);
                    strcpy(pc_add,three_zero);
                    strcpy(three_zero,"000");
                }
            else if(strlen(pc_add)==4)
                {
                    strcat(four_zero,pc_add);
                    strcpy(pc_add,four_zero);
                    strcpy(four_zero,"0000");
                }
            else if(strlen(pc_add)==3)
                {
                    strcat(five_zero,pc_add);
                    strcpy(pc_add,five_zero);
                    strcpy(five_zero,"00000");
                }
            else if(strlen(pc_add)==2)
                {
                    strcat(six_zero,pc_add);
                    strcpy(pc_add,six_zero);
                    strcpy(six_zero,"000000");
                }
            else if(strlen(pc_add)==1)
                {
                    strcat(seven_zero,pc_add);
                    strcpy(pc_add,seven_zero);
                    strcpy(seven_zero,"0000000");
                }
            else if(strlen(pc_add)==0)
                {
                    printf("No address present at location %ld of the input file",line);
                    printf("Exiting...........");
                    exit(EXIT_FAILURE);
                }
							for(i=0;i<=strlen(pc_add)-1;i++)
	                {
	                    if(pc_add[i]=='0') strcpy(pc_add_temp_nibble,"0000");
	                    else if(pc_add[i]=='1') strcpy(pc_add_temp_nibble,"0001");
	                    else if(pc_add[i]=='2') strcpy(pc_add_temp_nibble,"0010");
	                    else if(pc_add[i]=='3') strcpy(pc_add_temp_nibble,"0011");
	                    else if(pc_add[i]=='4') strcpy(pc_add_temp_nibble,"0100");
	                    else if(pc_add[i]=='5') strcpy(pc_add_temp_nibble,"0101");
	                    else if(pc_add[i]=='6') strcpy(pc_add_temp_nibble,"0110");
	                    else if(pc_add[i]=='7') strcpy(pc_add_temp_nibble,"0111");
	                    else if(pc_add[i]=='8') strcpy(pc_add_temp_nibble,"1000");
	                    else if(pc_add[i]=='9') strcpy(pc_add_temp_nibble,"1001");
	                    else if(pc_add[i]=='a') strcpy(pc_add_temp_nibble,"1010");
	                    else if(pc_add[i]=='b') strcpy(pc_add_temp_nibble,"1011");
	                    else if(pc_add[i]=='c') strcpy(pc_add_temp_nibble,"1100");
	                    else if(pc_add[i]=='d') strcpy(pc_add_temp_nibble,"1101");
	                    else if(pc_add[i]=='e') strcpy(pc_add_temp_nibble,"1110");
	                    else if(pc_add[i]=='f') strcpy(pc_add_temp_nibble,"1111");
	                    else
	                        {
	                            printf("Invalid memory address at line %ld of input file\n",line);
	                            printf("Exiting.........\n");
	                            exit(EXIT_FAILURE);
	                        }
	                    //puts(pc_add_temp_nibble);
	                    strcat(pc_add_binary,pc_add_temp_nibble);
	                }
						//printf("PC add: %s\n",pc_add);
						//printf("PC add in binary: %s\n",pc_add_binary);
						//printf("Status: %c\n",status);

//-------------------------if branch_prediction_type is bimodal-------------------------------------------


//-------------------------if branch_prediction_type is gshare-------------------------------------------

						pc_add_binary_offset_for_n = 32 - (m1_parameter + 2 );
            pc_add_binary_offset_for_m1_minus_n = 32 - ((m1_parameter-n_parameter) + 2);

						strncpy(branch_index_binary_excluding_n,pc_add_binary+pc_add_binary_offset_for_m1_minus_n,m1_parameter-n_parameter);
            strncpy(branch_history_binary,pc_add_binary+pc_add_binary_offset_for_n,n_parameter);
            //printf("index excluding n: %s\n",branch_index_binary_excluding_n);
            //printf("local branch history in binary: %s\n",branch_history_binary);
            /*
            for(i=0;i<n_parameter;i++)
            {
              printf("GHBR bit %d : %d\n",i, global_branch_history_register[i]-48);

            }

            for(i=0;i<n_parameter;i++)
            {
              printf("local BHR bit %d : %d\n",i, branch_history_binary[i]-48);

            }
            */
            for(i=0;i<n_parameter;i++)
            {
              xor_result[i] = ((global_branch_history_register[i]-48) ^ (branch_history_binary[i]-48));
              //printf("Xor result bit %d : %d\n",i, xor_result[i]);

            }

            for(i=0;i<n_parameter;i++)
            {
              sprintf(&branch_history_binary[i], "%d", xor_result[i]);
            }
            //printf("\nglobal_branch_history_register updated: %s\n\n",global_branch_history_register);
            //printf("updated ghbr: %s\n",xor_result);
            strcpy(branch_index_binary, branch_history_binary);
            strcat(branch_index_binary, branch_index_binary_excluding_n);
            //printf("New index reg value: %s\n",branch_index_binary);

            if(status == TAKEN)
            {
              for(i=n_parameter;i>0;i--)
              {
                global_branch_history_register[i] = global_branch_history_register[i-1];
              }
              global_branch_history_register[0] = '1';
              for(i=0;i<n_parameter;i++)
              {
                //printf("Updated GHBR result bit %d : %c\n",i, global_branch_history_register[i]);
              }
            }
            else if(status == NOT_TAKEN)
            {
              for(i=n_parameter;i>0;i--)
              {
                global_branch_history_register[i] = global_branch_history_register[i-1];
              }
              global_branch_history_register[0] = '0';
              for(i=0;i<n_parameter;i++)
              {
                //printf("Updated GHBR result bit %d : %c\n",i, global_branch_history_register[i]);
              }
            }
            //printf("branch_index_binary: %s", (branch_index_binary));
            /*
            while(atoi(branch_index_binary) != 0)
            {
                branch_index_decimal = branch_index_decimal + base*(atoi(branch_index_binary)%10);
                branch_index_decimal_div_10 = atoi(branch_index_binary) / 10;
                base=base*2;
                snprintf(branch_index_binary, m1_parameter, "%d", branch_index_decimal_div_10);
            }
            base=1;*/
            branch_index_decimal = 0;
            for(i=m1_parameter;i>=0;i--)
            {
              if(branch_index_binary[i] == '1')
              {
                branch_index_decimal = branch_index_decimal + pow(2,m1_parameter-i-1);
              }
              else if(branch_index_binary[i] == '0')
              {
                  branch_index_decimal = branch_index_decimal;
              }
            }
						//printf("Branch add in decimal: %d\n",branch_index_decimal);

            //-----------Start routine for prediction_table counters-------------------------//

            if( (status == TAKEN) )
						{
							//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
							//printf("Correct prediction\n");

							if(prediction_table[branch_index_decimal]<2) incorrect_predictions++;
							else if(prediction_table[branch_index_decimal]>=2) correct_predictions++;

							if(prediction_table[branch_index_decimal]<3) prediction_table[branch_index_decimal]++;
							//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
						}
						else if( (status == NOT_TAKEN))
						{
							//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
							//printf("Incorrect prediction!\n");

							if(prediction_table[branch_index_decimal]<2) correct_predictions++;
							else if(prediction_table[branch_index_decimal]>=2) incorrect_predictions++;

							if(prediction_table[branch_index_decimal]>0) prediction_table[branch_index_decimal]--;
							//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
						}

            branch_index_decimal = 0;
						memset(pc_add_temp_nibble,0,sizeof(pc_add_temp_nibble));
            memset(pc_add_binary,0,sizeof(pc_add_binary));
						memset(branch_index_binary,0,sizeof(branch_index_binary));
						memset(pc_add,0,sizeof(pc_add));

						line++;


				}
				//while eof ends
				printf("correct_predictions: %d\n",correct_predictions);
				printf("incorrect_predictions: %d\n",incorrect_predictions);
				printf("misprediction rate: %f",float(incorrect_predictions)/(float(incorrect_predictions)+float(correct_predictions)));
				fclose(trace_file);

				return 0;

	}
