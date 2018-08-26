
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#define TAKEN     ((const unsigned char)'t')
#define NOT_TAKEN     ((const unsigned char)'n')
#define VALID 1
#define INVALID 0

int main(int argc, char *argv[])

	{
		FILE *trace_file;
//char chara;
		float w_btb_misprediction_rate=0.00, wo_btb_misprediction_rate=0.00;
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
		int branch_index_decimal=0, bimodal_branch_index_decimal=0, gshare_branch_index_decimal=0;
		int branch_index_decimal_div_10=0;
		int pc_add_binary_offset_for_m2=0;
    int pc_add_binary_offset_for_m1_minus_n=0;
    int pc_add_binary_offset_for_n=0;
		int pc_add_binary_offset_for_k=0;
		int bimodal_predicted_flag=0, gshare_predicted_flag=0;

		char pc_add_binary[33] = "";
		char branch_index_binary[33] = "", bimodal_branch_index_binary[33] = "", gshare_branch_index_binary[33] = "";
    char branch_index_binary_excluding_n[33] = "", gshare_branch_index_binary_excluding_n[33] = "";
    char branch_history_binary[33] = "", gshare_branch_history_binary[33] = "";

		char one_zero[9] = "0";
    char two_zero[9] = "00";
    char three_zero[9] = "000";
    char four_zero[9] = "0000";
    char five_zero[9] = "00000";
    char six_zero[9] = "000000";
    char seven_zero[9] = "0000000";

		int *prediction_table = NULL;
		int *bimodal_prediction_table = NULL;
		int *gshare_prediction_table = NULL;
		int *hybrid_prediction_table = NULL;
    char (*global_branch_history_register) = NULL;
    int *xor_result=NULL;
    //char zero = "0";
		//-----btb variables-----//
		int j=0, temp1=0, temp2=0, temp3=0, temp4=0, hit_flag=0;
		int btb_block_size = 4;
		int btb_hits=0;
		int btb_index_decimal=0;
		int btb_lines=0, btb_line_index=0, btb_block_offset=0, btb_tag=0;
		int btb_incorrect_predictions=0;
		long hex_equ=0;
		char btb_tag_bits[33]="";
		char btb_index_binary[33]="";

		char (*btb_tag_arrays)[33]=NULL;
		int *valid_bit_array=NULL;
		int *lru_counter=NULL;


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
			if(btb_size !=0 && btb_assoc !=0)
			{
				btb_lines = btb_size/(btb_assoc*btb_block_size);
				btb_line_index = floor(log2(btb_lines));
				btb_block_offset = floor(log2(btb_block_size)); // 2
				btb_tag = 32 - (btb_block_offset + btb_line_index);
				btb_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*btb_lines*btb_assoc);
        if(btb_tag_arrays == NULL) printf("BTB init failed!\n");
        //else printf("Cache init success!\n");
        //initialising the tag array with zeros
        for(i=0;i<btb_assoc;i++)
        {
            for(j=0;j<btb_lines;j++)
            {
                memset(btb_tag_arrays[(i*btb_lines) + j], 0, 33*sizeof(char));

            }
        }
				valid_bit_array = (int*)malloc(sizeof(int)*btb_lines);
				if(valid_bit_array == NULL) printf("BTB Valid bit init failed\n");
				//initialise to invalid
				for(i=0;i<btb_assoc;i++)
        {
            for(j=0;j<btb_lines;j++)
            {
                valid_bit_array[(i*btb_lines) + j] = INVALID;

            }
        }

				lru_counter = (int*)malloc(sizeof(int)*btb_lines*btb_assoc);
        if(lru_counter == NULL) printf("BTB LRU counter init failed!\n");
        //else printf("LRU counter init success!\n");
        //adding random values to lru_counter for initial setup
        temp1 = btb_assoc-1;
        for(i=0;i<btb_assoc;i++)
        {
            for(j=0;j<btb_lines;j++)
            {
                lru_counter[(i*btb_lines) + j]=temp1;

            }
            temp1 = temp1 - 1;
        }
        temp1=0;

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

			if(btb_size !=0 && btb_assoc !=0)
			{
				btb_lines = btb_size/(btb_assoc*btb_block_size);
				btb_line_index = floor(log2(btb_lines));
				btb_block_offset = floor(log2(btb_block_size)); // 2
				btb_tag = 32 - (btb_block_offset + btb_line_index);
				btb_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*btb_lines*btb_assoc);
        if(btb_tag_arrays == NULL) printf("BTB init failed!\n");
        //else printf("Cache init success!\n");
        //initialising the tag array with zeros
        for(i=0;i<btb_assoc;i++)
        {
            for(j=0;j<btb_lines;j++)
            {
                memset(btb_tag_arrays[(i*btb_lines) + j], 0, 33*sizeof(char));

            }
        }
				valid_bit_array = (int*)malloc(sizeof(int)*btb_lines);
				if(valid_bit_array == NULL) printf("BTB Valid bit init failed\n");
				//initialise to invalid
				for(i=0;i<btb_assoc;i++)
        {
            for(j=0;j<btb_lines;j++)
            {
                valid_bit_array[(i*btb_lines) + j] = INVALID;

            }
        }

				lru_counter = (int*)malloc(sizeof(int)*btb_lines*btb_assoc);
        if(lru_counter == NULL) printf("BTB LRU counter init failed!\n");
        //else printf("LRU counter init success!\n");
        //adding random values to lru_counter for initial setup
        temp1 = btb_assoc-1;
        for(i=0;i<btb_assoc;i++)
        {
            for(j=0;j<btb_lines;j++)
            {
                lru_counter[(i*btb_lines) + j]=temp1;

            }
            temp1 = temp1 - 1;
        }
        temp1=0;

			}
    }

		//only assign if branch_prediction_type is hybrid
		if(strcmp(branch_prediction_type,"hybrid")==0)
		{
			//printf("inside hybrid\n");
			k_parameter = atoi(argv[2]);
			m1_parameter = atoi(argv[3]);
			n_parameter = atoi(argv[4]);
			m2_parameter = atoi(argv[5]);
			btb_size = atoi(argv[6]);
			btb_assoc = atoi(argv[7]);
			trace_file = fopen(argv[8], "r");

			hybrid_prediction_table = (int*)malloc(sizeof(int)*(pow(2,k_parameter)));
			for(i=0;i<(pow(2,k_parameter));i++)
  		{
  			hybrid_prediction_table[i] = 1;
  			//printf("prediction_table value at %d: %d\n",i, prediction_table[i]);
  		}

			bimodal_prediction_table = (int*)malloc(sizeof(int)*(pow(2,m2_parameter)));
			for(i=0;i<(pow(2,m2_parameter));i++)
  		{
  			bimodal_prediction_table[i] = 2;
  			//printf("prediction_table value at %d: %d\n",i, prediction_table[i]);
  		}

			gshare_prediction_table = (int*)malloc(sizeof(int)*(pow(2,m1_parameter)));
      for(i=0;i<(pow(2,m1_parameter));i++)
  		{
  			gshare_prediction_table[i] = 2;
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

		fseek(trace_file, 0, SEEK_SET);

  while ((cursor=fgetc(trace_file)) != EOF)
      {
          fseek(trace_file, -1, SEEK_CUR);
          fscanf(trace_file, "%s %c\n", pc_add, &status);
//					printf("PC add: %s\n",pc_add);
//					printf("Status: %d\n",status);
//					printf("Status: %c\n",chara);
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
					//printf("%d:PC add: %s\t",line,pc_add);
					//printf("PC add in binary: %s\n",pc_add_binary);
					//printf("Status: %c\n",status);
					//--------------------------Bimodal routine--------------------------------------///
				if(strcmp(branch_prediction_type,"bimodal")==0)
				{

					pc_add_binary_offset_for_m2 = 32 - (m2_parameter + 2);

					strncpy(branch_index_binary,pc_add_binary+pc_add_binary_offset_for_m2,m2_parameter);
					//printf("Branch add in binary: %s\n",branch_index_binary);

					for(i=m2_parameter;i>=0;i--)
          {
            if(branch_index_binary[i] == '1')
            {
              branch_index_decimal = branch_index_decimal + pow(2,m2_parameter-i-1);
            }
            else if(branch_index_binary[i] == '0')
            {
               //nothing to do
            }
          }
					//printf("Branch add in decimal: %d\n",branch_index_decimal);

					if(btb_size != 0 && btb_assoc !=0)
					{
						//find tag string  and index string from the address read
						strncpy(btb_index_binary, pc_add_binary+btb_tag,btb_line_index);
						//printf("btb_index_binary: %s\n",btb_index_binary);
						for(i=btb_line_index;i>=0;i--)
	          {
	            if(btb_index_binary[i] == '1')
	            {
	              btb_index_decimal = btb_index_decimal + pow(2,btb_line_index-i-1);
	            }
	            else if(btb_index_binary[i] == '0')
	            {
	               //nothing to do
	            }
	          }
						//printf("BTB index in decimal: %d\n",btb_index_decimal);
						strncpy(btb_tag_bits,pc_add_binary,btb_tag);
						//printf("btb tag bits in binary: %s\n",btb_tag_bits);

						for(j=0;j<btb_assoc;j++)
						{
								if(strcmp(btb_tag_arrays[(j*btb_lines) + btb_index_decimal],btb_tag_bits)==0)
								{
										hit_flag = 1;
										temp1 = j;
										temp2 = lru_counter[(j*btb_lines) + btb_index_decimal];
										break;
								}
								else hit_flag =0;
						}

						if(hit_flag == 1)
						{
							//printf("BTB Hit! at set %d of line %d\n",temp1, btb_index_decimal);
							btb_hits++;
							lru_counter[(temp1*btb_lines) + btb_index_decimal] = 0;
							for(j=0;j<btb_assoc;j++)
							{
									if(j!=temp1)
									{
											if((lru_counter[(j*btb_lines) + btb_index_decimal]) < temp2)
											{
													lru_counter[(j*btb_lines) + btb_index_decimal]++;
													//printf("incremented LRU counter of set %d\n",j);
											}
									}

							}

							if(status == TAKEN)
							{
								//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
								//printf("Correct prediction\n");

								if(prediction_table[branch_index_decimal]<2) incorrect_predictions++;
								else if(prediction_table[branch_index_decimal]>=2) correct_predictions++;

								if(prediction_table[branch_index_decimal]<3) prediction_table[branch_index_decimal]++;
								//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
							}
							else if(status == NOT_TAKEN)
							{
								//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
								//printf("Incorrect prediction!\n");

								if(prediction_table[branch_index_decimal]<2) correct_predictions++;
								else if(prediction_table[branch_index_decimal]>=2) incorrect_predictions++;

								if(prediction_table[branch_index_decimal]>0) prediction_table[branch_index_decimal]--;
								//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
							}


						}
						else if(hit_flag == 0)
						{
							temp3 = 0;
							temp4 = 0;
							//searching for the highest LRU counter value and setting it to zero
							for(j=0;j<btb_assoc;j++)
							{
									if(lru_counter[(j*btb_lines) + btb_index_decimal] > temp3)
									{
											temp3 = lru_counter[(j*btb_lines) + btb_index_decimal];
											temp4 = j;
									}
							}

							strcpy(btb_tag_arrays[(temp4*btb_lines) + btb_index_decimal],btb_tag_bits);
							if(strcpy(btb_tag_arrays[(temp4*btb_lines) + btb_index_decimal],btb_tag_bits) ==NULL) exit(EXIT_FAILURE);
							lru_counter[(temp4*btb_lines) + btb_index_decimal] = 0;
							//except for the recently modified lru counter, rest all counters are incremented
							for(j=0;j<btb_assoc;j++)
							{
									if(j!=temp4)
									lru_counter[(j*btb_lines) + btb_index_decimal]++;
							}

							if(status == TAKEN) btb_incorrect_predictions++;


						}
					}
					else if(btb_size == 0 && btb_assoc == 0)
					{
						if(status == TAKEN)
						{
							//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
							//printf("Correct prediction\n");

							if(prediction_table[branch_index_decimal]<2) incorrect_predictions++;
							else if(prediction_table[branch_index_decimal]>=2) correct_predictions++;

							if(prediction_table[branch_index_decimal]<3) prediction_table[branch_index_decimal]++;
							//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
						}
						else if(status == NOT_TAKEN)
						{
							//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
							//printf("Incorrect prediction!\n");

							if(prediction_table[branch_index_decimal]<2) correct_predictions++;
							else if(prediction_table[branch_index_decimal]>=2) incorrect_predictions++;

							if(prediction_table[branch_index_decimal]>0) prediction_table[branch_index_decimal]--;
							//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
						}
					}

					else
					{
						printf("Incorrect parameters found in command!\n");
						printf("Exiting..........\n");
						exit(EXIT_FAILURE);
					}


				}

					//--------------------------Gshare routine--------------------------------------///

				if(strcmp(branch_prediction_type,"gshare")==0)
				{

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

					for(i=m1_parameter;i>=0;i--)
          {
            if(branch_index_binary[i] == '1')
            {
              branch_index_decimal = branch_index_decimal + pow(2,m1_parameter-i-1);
            }
            else if(branch_index_binary[i] == '0')
            {
                //nothing to do
            }
          }

					if(btb_size != 0 && btb_assoc !=0)
					{
						//find tag string  and index string from the address read
						strncpy(btb_index_binary, pc_add_binary+btb_tag,btb_line_index);
						//printf("btb_index_binary: %s\n",btb_index_binary);
						for(i=btb_line_index;i>=0;i--)
	          {
	            if(btb_index_binary[i] == '1')
	            {
	              btb_index_decimal = btb_index_decimal + pow(2,btb_line_index-i-1);
	            }
	            else if(btb_index_binary[i] == '0')
	            {
	               //nothing to do
	            }
	          }
						//printf("BTB index in decimal: %d\n",btb_index_decimal);
						strncpy(btb_tag_bits,pc_add_binary,btb_tag);
						//printf("btb tag bits in binary: %s\n",btb_tag_bits);

						for(j=0;j<btb_assoc;j++)
						{
								if(strcmp(btb_tag_arrays[(j*btb_lines) + btb_index_decimal],btb_tag_bits)==0)
								{
										hit_flag = 1;
										temp1 = j;
										temp2 = lru_counter[(j*btb_lines) + btb_index_decimal];
										break;
								}
								else hit_flag =0;
						}

						if(hit_flag == 1)
						{
							//printf("BTB Hit! at set %d of line %d\n",temp1, btb_index_decimal);
							btb_hits++;
							lru_counter[(temp1*btb_lines) + btb_index_decimal] = 0;
							for(j=0;j<btb_assoc;j++)
							{
									if(j!=temp1)
									{
											if((lru_counter[(j*btb_lines) + btb_index_decimal]) < temp2)
											{
													lru_counter[(j*btb_lines) + btb_index_decimal]++;
													//printf("incremented LRU counter of set %d\n",j);
											}
									}

							}
							//do regular gshare routine

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
							if(status == TAKEN)
							{
								//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
								//printf("Correct prediction\n");

								if(prediction_table[branch_index_decimal]<2) incorrect_predictions++;
								else if(prediction_table[branch_index_decimal]>=2) correct_predictions++;

								if(prediction_table[branch_index_decimal]<3) prediction_table[branch_index_decimal]++;
								//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
							}
							else if(status == NOT_TAKEN)
							{
								//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
								//printf("Incorrect prediction!\n");

								if(prediction_table[branch_index_decimal]<2) correct_predictions++;
								else if(prediction_table[branch_index_decimal]>=2) incorrect_predictions++;

								if(prediction_table[branch_index_decimal]>0) prediction_table[branch_index_decimal]--;
								//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
							}

						}

						else if(hit_flag == 0)
						{
							temp3 = 0;
							temp4 = 0;
							//searching for the highest LRU counter value and setting it to zero
							for(j=0;j<btb_assoc;j++)
							{
									if(lru_counter[(j*btb_lines) + btb_index_decimal] > temp3)
									{
											temp3 = lru_counter[(j*btb_lines) + btb_index_decimal];
											temp4 = j;
									}
							}

							strcpy(btb_tag_arrays[(temp4*btb_lines) + btb_index_decimal],btb_tag_bits);
							if(strcpy(btb_tag_arrays[(temp4*btb_lines) + btb_index_decimal],btb_tag_bits) ==NULL) exit(EXIT_FAILURE);
							lru_counter[(temp4*btb_lines) + btb_index_decimal] = 0;
							//except for the recently modified lru counter, rest all counters are incremented
							for(j=0;j<btb_assoc;j++)
							{
									if(j!=temp4)
									lru_counter[(j*btb_lines) + btb_index_decimal]++;
							}

							if(status == TAKEN) btb_incorrect_predictions++;
						}

					}
					else if(btb_size == 0 && btb_assoc ==0)
					{
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
						if(status == TAKEN)
						{
							//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
							//printf("Correct prediction\n");

							if(prediction_table[branch_index_decimal]<2) incorrect_predictions++;
							else if(prediction_table[branch_index_decimal]>=2) correct_predictions++;

							if(prediction_table[branch_index_decimal]<3) prediction_table[branch_index_decimal]++;
							//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
						}
						else if(status == NOT_TAKEN)
						{
							//printf("Counter value: %d ",prediction_table[branch_index_decimal]);
							//printf("Incorrect prediction!\n");

							if(prediction_table[branch_index_decimal]<2) correct_predictions++;
							else if(prediction_table[branch_index_decimal]>=2) incorrect_predictions++;

							if(prediction_table[branch_index_decimal]>0) prediction_table[branch_index_decimal]--;
							//printf("Updated Counter value: %d\n",prediction_table[branch_index_decimal]);
						}
					}

					else
					{
						printf("Incorrect parameters found in command!\n");
						printf("Exiting..........\n");
						exit(EXIT_FAILURE);
					}
				}

					//-------------------------Hybrid routine----------------------------------------//

				if(strcmp(branch_prediction_type,"hybrid")==0)
				{
					pc_add_binary_offset_for_k = 32 - (k_parameter + 2);
					strncpy(branch_index_binary,pc_add_binary+pc_add_binary_offset_for_k,k_parameter);

					for(i=k_parameter;i>=0;i--)
          {
            if(branch_index_binary[i] == '1')
            {
              branch_index_decimal = branch_index_decimal + pow(2,k_parameter-i-1);
            }
            else if(branch_index_binary[i] == '0')
            {
               //nothing to do
            }
          }
					//printf("chooser index: %d\n", branch_index_decimal);
					//calculate decimal index bits for prediction_table of bimodal
					pc_add_binary_offset_for_m2 = 32 - (m2_parameter + 2);
					strncpy(bimodal_branch_index_binary,pc_add_binary+pc_add_binary_offset_for_m2,m2_parameter);

					for(i=m2_parameter;i>=0;i--)
          {
            if(bimodal_branch_index_binary[i] == '1')
            {
              bimodal_branch_index_decimal = bimodal_branch_index_decimal + pow(2,m2_parameter-i-1);
            }
            else if(bimodal_branch_index_binary[i] == '0')
            {
               //nothing to do
            }
          }
					//printf("bimodal index: %d\n", bimodal_branch_index_decimal);
					//calculate decimal index bits for prediction_table of gshare
					pc_add_binary_offset_for_n = 32 - (m1_parameter + 2 );
          pc_add_binary_offset_for_m1_minus_n = 32 - ((m1_parameter-n_parameter) + 2);

					strncpy(gshare_branch_index_binary_excluding_n,pc_add_binary+pc_add_binary_offset_for_m1_minus_n,m1_parameter-n_parameter);
          strncpy(gshare_branch_history_binary,pc_add_binary+pc_add_binary_offset_for_n,n_parameter);
          //printf("index excluding n: %s\n",gshare_branch_index_binary_excluding_n);
          //printf("local branch history in binary: %s\n",gshare_branch_history_binary);
          /*
          for(i=0;i<n_parameter;i++)
          {
            printf("GHBR bit %d : %d\n",i, global_branch_history_register[i]-48);

          }

          for(i=0;i<n_parameter;i++)
          {
            printf("local BHR bit %d : %d\n",i, gshare_branch_history_binary[i]-48);

          }
          */
          for(i=0;i<n_parameter;i++)
          {
            xor_result[i] = ((global_branch_history_register[i]-48) ^ (gshare_branch_history_binary[i]-48));
            //printf("Xor result bit %d : %d\n",i, xor_result[i]);

          }

          for(i=0;i<n_parameter;i++)
          {
            sprintf(&gshare_branch_history_binary[i], "%d", xor_result[i]);
          }
          //printf("\nglobal_branch_history_register updated: %s\n\n",global_branch_history_register);
          //printf("updated ghbr: %s\n",xor_result);
          strcpy(gshare_branch_index_binary, gshare_branch_history_binary);
          strcat(gshare_branch_index_binary, gshare_branch_index_binary_excluding_n);
          //printf("New index reg value: %s\n",gshare_branch_index_binary);

					for(i=m1_parameter;i>=0;i--)
          {
            if(gshare_branch_index_binary[i] == '1')
            {
              gshare_branch_index_decimal = gshare_branch_index_decimal + pow(2,m1_parameter-i-1);
            }
            else if(gshare_branch_index_binary[i] == '0')
            {
                //nothing to do
            }
          }
					//printf("gshare index: %d\n", gshare_branch_index_decimal);

					if(status==TAKEN)
					{
						if(bimodal_prediction_table[bimodal_branch_index_decimal]>=2) bimodal_predicted_flag = 1;
						else if(bimodal_prediction_table[bimodal_branch_index_decimal]<2) bimodal_predicted_flag = 0;

						if(gshare_prediction_table[gshare_branch_index_decimal]>=2) gshare_predicted_flag = 1;
						else if(gshare_prediction_table[gshare_branch_index_decimal]<2) gshare_predicted_flag = 0;

					}

					if(status==NOT_TAKEN)
					{
						if(bimodal_prediction_table[bimodal_branch_index_decimal]>=2) bimodal_predicted_flag = 0;
						else if(bimodal_prediction_table[bimodal_branch_index_decimal]<2) bimodal_predicted_flag = 1;

						if(gshare_prediction_table[gshare_branch_index_decimal]>=2) gshare_predicted_flag = 0;
						else if(gshare_prediction_table[gshare_branch_index_decimal]<2) gshare_predicted_flag = 1;

					}

					if(hybrid_prediction_table[branch_index_decimal] >=2)
					{
						//select gshare predictor
						//printf("Gshare selected\n");
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
	             // printf("Updated GHBR result bit %d : %c\n",i, global_branch_history_register[i]);
	            }
	          }

						if(status == TAKEN)
						{
							//printf("gshare Counter value: %d ",gshare_prediction_table[gshare_branch_index_decimal]);
							//printf("Correct prediction\n");

							if(gshare_prediction_table[gshare_branch_index_decimal]<2) incorrect_predictions++;
							else if(gshare_prediction_table[gshare_branch_index_decimal]>=2) correct_predictions++;

							if(gshare_prediction_table[gshare_branch_index_decimal]<3) gshare_prediction_table[gshare_branch_index_decimal]++;
							//printf("gshare Updated Counter value: %d\n",gshare_prediction_table[gshare_branch_index_decimal]);
						}
						else if(status == NOT_TAKEN)
						{
							//printf("gshare Counter value: %d ",gshare_prediction_table[gshare_branch_index_decimal]);
							//printf("Incorrect prediction!\n");

							if(gshare_prediction_table[gshare_branch_index_decimal]<2) correct_predictions++;
							else if(gshare_prediction_table[gshare_branch_index_decimal]>=2) incorrect_predictions++;

							if(gshare_prediction_table[gshare_branch_index_decimal]>0) gshare_prediction_table[gshare_branch_index_decimal]--;
							//printf("gshare Updated Counter value: %d\n",gshare_prediction_table[gshare_branch_index_decimal]);
						}

					}
					else if(hybrid_prediction_table[branch_index_decimal]<2)
					{
						//select bimodal predictor and update GHBR too
						//printf("Bimodal selected\n");
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
	             // printf("Updated GHBR result bit %d : %c\n",i, global_branch_history_register[i]);
	            }
	          }

						if(status == TAKEN)
						{
							//printf("bimodal Counter value: %d ",bimodal_prediction_table[bimodal_branch_index_decimal]);
							//printf("Correct prediction\n");

							if(bimodal_prediction_table[bimodal_branch_index_decimal]<2) incorrect_predictions++;
							else if(bimodal_prediction_table[bimodal_branch_index_decimal]>=2) correct_predictions++;

							if(bimodal_prediction_table[bimodal_branch_index_decimal]<3) bimodal_prediction_table[bimodal_branch_index_decimal]++;
							//printf("bimodal Updated Counter value: %d\n",bimodal_prediction_table[bimodal_branch_index_decimal]);
						}
						else if(status == NOT_TAKEN)
						{
							//printf("bimodal Counter value: %d ",bimodal_prediction_table[bimodal_branch_index_decimal]);
							//printf("Incorrect prediction!\n");

							if(bimodal_prediction_table[bimodal_branch_index_decimal]<2) correct_predictions++;
							else if(bimodal_prediction_table[bimodal_branch_index_decimal]>=2) incorrect_predictions++;

							if(bimodal_prediction_table[bimodal_branch_index_decimal]>0) bimodal_prediction_table[bimodal_branch_index_decimal]--;
							//printf("bimodal Updated Counter value: %d\n",bimodal_prediction_table[bimodal_branch_index_decimal]);
						}

					}

					//now update hybrid counter value

					if(bimodal_predicted_flag == 0 && gshare_predicted_flag == 1)
					{
						if(hybrid_prediction_table[branch_index_decimal]<3) hybrid_prediction_table[branch_index_decimal]++;
					}
					else if(bimodal_predicted_flag == 1 && gshare_predicted_flag == 0)
					{
						if(hybrid_prediction_table[branch_index_decimal]>0) hybrid_prediction_table[branch_index_decimal]--;
					}

				}

				branch_index_decimal = 0;
				btb_index_decimal=0;
				bimodal_branch_index_decimal = 0;
				gshare_branch_index_decimal = 0;
				memset(pc_add_temp_nibble,0,sizeof(pc_add_temp_nibble));
        memset(pc_add_binary,0,sizeof(pc_add_binary));
				memset(branch_index_binary,0,sizeof(branch_index_binary));
				memset(bimodal_branch_index_binary,0,sizeof(branch_index_binary));
				memset(gshare_branch_index_binary,0,sizeof(branch_index_binary));
				memset(gshare_branch_history_binary,0,sizeof(gshare_branch_history_binary));
				memset(branch_history_binary,0,sizeof(branch_history_binary));
				memset(pc_add,0,sizeof(pc_add));

				line++;

			}
				//while eof ends
				//free the pointers
				wo_btb_misprediction_rate = float(incorrect_predictions)/float(line-1);
				w_btb_misprediction_rate = float(incorrect_predictions+btb_incorrect_predictions)/float(line-1);
				/*
				printf("COMMAND\n");
				if((strcmp(branch_prediction_type,"bimodal")==0) && btb_size == 0 && btb_assoc == 0)
				{
					printf("./sim %s %d %d %d %s\n",branch_prediction_type, m2_parameter, btb_size, btb_assoc, argv[5]);
					printf("OUTPUT\n");
					printf("number of predictions: %d\n",line-1);
					printf("number of mispredictions: %d\n", incorrect_predictions);
					printf("misprediction rate: %0.2f%%\n",100*wo_btb_misprediction_rate);
					printf("FINAL BIMODAL CONTENTS\n");
					for(i=0;i<(pow(2,m2_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,prediction_table[i]);
		  		}

				}
				if((strcmp(branch_prediction_type,"bimodal")==0) && btb_size !=0 && btb_assoc !=0)
				{
					printf("./sim %s %d %d %d %s\n",branch_prediction_type, m2_parameter, btb_size, btb_assoc, argv[5]);
					printf("OUTPUT\n");
					printf("size of BTB: %d\n",btb_size);
					printf("number of branches: %d\n",line-1);
					printf("number of predictions from branch predictor: %d\n",btb_hits);
					printf("number of mispredictions from branch predictor: %d\n", incorrect_predictions);
					printf("number of branches miss in BTB and taken: %d\n", btb_incorrect_predictions);
					printf("total mispredictions: %d\n", (incorrect_predictions+btb_incorrect_predictions));
					printf("misprediction rate: %0.2f%%\n",100*w_btb_misprediction_rate);
					printf("\nFINAL BTB CONTENTS\n");
					for(j=0;j<btb_lines;j++)
					{
							printf("Set %d: ",j);
							for(i=0;i<btb_assoc;i++)
							{
									hex_equ = strtol(btb_tag_arrays[(i*btb_lines) + j],NULL,2);
									printf("%lx ",hex_equ);
									//if (sm_dirty_bit[(i*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
							}
							printf("\n");

					}
					printf("\nFINAL BIMODAL CONTENTS\n");
					for(i=0;i<(pow(2,m2_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,prediction_table[i]);
		  		}

				}
				if((strcmp(branch_prediction_type,"gshare")==0) && btb_size == 0 && btb_assoc == 0)
				{
					printf("./sim %s %d %d %d %d %s\n",branch_prediction_type, m1_parameter, n_parameter, btb_size, btb_assoc, argv[6]);
					printf("OUTPUT\n");
					printf("number of predictions: %d\n",line-1);
					printf("number of mispredictions: %d\n", incorrect_predictions);
					printf("misprediction rate: %0.2f%%\n",100*wo_btb_misprediction_rate);
					printf("FINAL GSHARE CONTENTS\n");
					for(i=0;i<(pow(2,m1_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,prediction_table[i]);
		  		}

				}
				if((strcmp(branch_prediction_type,"gshare")==0) && btb_size !=0 && btb_assoc !=0)
				{
					printf("./sim %s %d %d %d %d %s\n",branch_prediction_type, m1_parameter, n_parameter, btb_size, btb_assoc, argv[6]);
					printf("OUTPUT\n");
					printf("size of BTB: %d\n",btb_size);
					printf("number of branches: %d\n",line-1);
					printf("number of predictions from branch predictor: %d\n",btb_hits);
					printf("number of mispredictions from branch predictor: %d\n", incorrect_predictions);
					printf("number of branches miss in BTB and taken: %d\n", btb_incorrect_predictions);
					printf("total mispredictions: %d\n", (incorrect_predictions+btb_incorrect_predictions));
					printf("misprediction rate: %0.2f%%\n",100*w_btb_misprediction_rate);
					printf("\nFINAL BTB CONTENTS\n");
					for(j=0;j<btb_lines;j++)
					{
							printf("Set %d: ",j);
							for(i=0;i<btb_assoc;i++)
							{
									hex_equ = strtol(btb_tag_arrays[(i*btb_lines) + j],NULL,2);
									printf("%lx ",hex_equ);
									//if (sm_dirty_bit[(i*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
							}
							printf("\n");

					}
					printf("\nFINAL GSHARE CONTENTS\n");
					for(i=0;i<(pow(2,m1_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,prediction_table[i]);
		  		}

				}
				if((strcmp(branch_prediction_type,"hybrid")==0) && btb_size == 0 && btb_assoc == 0)
				{
					printf("./sim %s %d %d %d %d %d %d %s\n",branch_prediction_type, k_parameter, m1_parameter, n_parameter, m2_parameter, btb_size, btb_assoc, argv[8]);
					printf("OUTPUT\n");
					printf("number of predictions: %d\n",line-1);
					printf("number of mispredictions: %d\n", incorrect_predictions);
					printf("misprediction rate: %0.2f%%\n",100*wo_btb_misprediction_rate);
					printf("FINAL CHOOSER CONTENTS\n");
					for(i=0;i<(pow(2,k_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,hybrid_prediction_table[i]);
		  		}
					printf("\nFINAL GSHARE CONTENTS\n");
					for(i=0;i<(pow(2,m1_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,gshare_prediction_table[i]);
		  		}
					printf("\nFINAL BIMODAL CONTENTS\n");
					for(i=0;i<(pow(2,m2_parameter));i++)
		  		{
		  			printf("%d  %d\n",i,bimodal_prediction_table[i]);
		  		}

				}
				*/

				//printf("branches: %d\n",line-1);
				//printf("btb hits: %d\n", btb_hits);
				//printf("incorrect_predictions: %d\n",incorrect_predictions);
				//printf("btb incorrect_predictions: %d\n",btb_incorrect_predictions);
				//printf("total misprediction: %d\n",btb_incorrect_predictions+incorrect_predictions);
				printf("misprediction rate: %f\n",(float(incorrect_predictions+btb_incorrect_predictions)/float(line-1)));
				/*
				for(j=0;j<btb_lines;j++)
				{
						printf("Set %d  : ",j);
						for(i=0;i<btb_assoc;i++)
						{
								hex_equ = strtol(btb_tag_arrays[(i*btb_lines) + j],NULL,2);
								printf("%lx ",hex_equ);
								//if (sm_dirty_bit[(i*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
						}
						printf("\n");

				}
				*/
				free(prediction_table);
				free(bimodal_prediction_table);
				free(gshare_prediction_table);
				free(hybrid_prediction_table);
		    free(global_branch_history_register);
		    free(xor_result);
				free(btb_tag_arrays);
				free(lru_counter);
				fclose(trace_file);
				return 0;

	}
