#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>

#define EPSILON 1.0000000001
#define CONSTANT 0.9
#define SPLIT_LEVEL 0.4 //percentage of max height <0.00;1.00>

enum {
	TETROMINO_SIZE = 4,
	TETROMINO_TYPES = 5,
	I_INDEX = 0,
	O_INDEX = 1,
	T_INDEX = 2,
	L_INDEX = 3,
	J_INDEX = 3,
	S_INDEX = 4,
	Z_INDEX = 4,
	HEIGHT = 7,
	WIDTH = 10, 
	SEED = 6
	//~ 13*6 ok, 7*9, 7*14
	//~ 11x6 = 33s, 1.0 rand, no srand
	//~ 13x6 = 73s, 1.0 rand, no srand
	//~ 11x6 = 248s, 0.9 rand, no srand
	
		//~ 11x6 = 278s, 0.9 rand, srand(0) 87s
		//~ 11x6 = 253s, 0.9 rand, srand(1)
			//~ 11x6 = 10s, 0.9 rand, srand(2) 10s
			//~ 11x6 = 137s, 0.9 rand, srand(3) 11s
	//~ 11x6 = ?s, 0.9 rand, srand(4) 700+
		//~ 11x6 = 87s, 0.9 rand, srand(5)
			//~ 11x6 = 4s, 0.9 rand, srand(6)
	//~ 11x6 = ?s+, 0.9 rand, srand(7) 1000+
		//~ 11x6 = 238s, 0.9 rand, srand(8)
	//~ 11x6 = ?s, 0.9 rand, srand(9) 1600+
	//~ 11x6 = ?s, 0.9 rand, srand(10) 1600+
		//~ 11x6 = 539s, 0.9 rand, srand(11)
	//~ 11x6 = 29s, 0.9 rand, srand(12)
	//~ 11x6 = 13s, 0.9 rand, srand(13)
	//~ 11x6 = ?s, 0.9 rand, srand(14)
	//~ 11x6 = ?s, 0.9 rand, srand(15)
		//~ 11x6 = 358s, 0.9 rand, srand(16)
	//~ 11x6 = 2s, 0.9 rand, srand(17)
	//~ 11x6 = ?s, 0.9 rand, srand(18)
		//~ 11x6 = 51s, 0.9 rand, srand(19)
	//~ 11x6 = 1s, 0.9 rand, srand(20)
	
	//~ 9 7 3 84s
	//~ 9 7 2 516s
	//~ 9 7 4 358s
	//~ 9 7 7 77s
	//~ 9 7 5 240s
	//~ 9 7 1 1170s
	
	//~ 9 7 1 1170s
	//~ 9 7 2 516s
	//~ 9 7 4 358s
	//~ 11x6 = 539s, 0.9 rand, srand(11)
	//~ 11x6 = 358s, 0.9 rand, srand(16)
};

struct Signature {
	unsigned char definition [5];
	int level;
	int disbalance;
	double price;
	bool corupted;
	int * parents;
	int * childs;
	int parents_cnt;
	int childs_cnt;
};

void fill_sig_def (Signature & sig,
                   unsigned char elem0 = 0,
                   unsigned char elem1 = 0,
                   unsigned char elem2 = 0,
                   unsigned char elem3 = 0,
                   unsigned char elem4 = 0) {
		sig.definition[0] = elem0;
		sig.definition[1] = elem1;
		sig.definition[2] = elem2;
		sig.definition[3] = elem3;
		sig.definition[4] = elem4;
		sig.level = elem0 + elem1 + elem2 + elem3 + elem4;
		sig.disbalance = sig.level - (5*elem4);
		sig.price = 100;
		sig.corupted = false;
		sig.parents = NULL;
		sig.childs = NULL;
		sig.parents_cnt = 0;
		sig.childs_cnt = 0;
	}

int generate_signatures(int max_depth, Signature signatures [], int sig_cnt) {
	unsigned combs = 0;

	for(int depth = 0; depth<=max_depth; depth++) {
		fill_sig_def(signatures[combs++], depth);
		//~ printf("%d ", depth);
		//~ combs++;
		for(int i = depth-1; i>=(depth+1)/2; i--) {
			fill_sig_def(signatures[combs++], i, depth-i);
			//~ printf("%d%d ", i, depth-i);
			//~ combs++;
		}
	
		for(int i = depth-2; i>=(depth+2)/3; i--) {
			for(int j = depth-i-1; j>=(depth-i+1)/2; j--) {
				if(i>=j) {
						fill_sig_def(signatures[combs++], i, j, depth - i - j);
						//~ printf("%d%d%d ", i, j, depth - i - j);
						//~ combs++;
					}
			}
		}
	
		for(int i = depth-3; i>=(depth+3)/4; i--) {
			for(int j = depth-i-2; j>=(depth-i+2)/3; j--) {
				if(i>=j) {
					for(int k = depth-i-j-1; k>=(depth-i-j+1)/2; k--) {
						if(j>=k) {
							fill_sig_def(signatures[combs++], i, j, k, depth - i - j - k);
							//~ printf("%d%d%d%d ", i, j, k, depth - i - j - k);
							//~ combs++;
						}
					}
				}
			}
		}
	
		for(int i = depth-4; i>=(depth+4)/5; i--) {
			for(int j = depth-i-3; j>=(depth-i+3)/4; j--) {
				if(i>=j) {
					for(int k = depth-i-j-2; k>=(depth-i-j+2)/3; k--) {
						if(j>=k) {
							for(int l = depth-i-j-k-1; l>=(depth-i-j-k+1)/2; l--) {
								if(k>=l) {
									fill_sig_def(signatures[combs++], i, j, k, l, depth - i - j - k - l);
									//~ printf("%d%d%d%d%d ", i, j, k, l, depth - i - j - k - l);
									//~ combs++;
								}
							}
						}
					}
				}
			}
		}
	
		//~ printf("\n");
	}
	//~ printf("%u\n", combs);
	return combs;
}

int count_signatures(int max_depth) {
	unsigned combs = 0;

	for(int depth = 0; depth<=max_depth; depth++) {
		//~ printf("%d ", depth);
		combs++;
		for(int i = depth-1; i>=(depth+1)/2; i--) {
			//~ printf("%d%d ", i, depth-i);
			combs++;
		}
	
		for(int i = depth-2; i>=(depth+2)/3; i--) {
			for(int j = depth-i-1; j>=(depth-i+1)/2; j--) {
				if(i>=j) {
						//~ printf("%d%d%d ", i, j, depth - i - j);
						combs++;
					}
			}
		}
	
		for(int i = depth-3; i>=(depth+3)/4; i--) {
			for(int j = depth-i-2; j>=(depth-i+2)/3; j--) {
				if(i>=j) {
					for(int k = depth-i-j-1; k>=(depth-i-j+1)/2; k--) {
						if(j>=k) {
							//~ printf("%d%d%d%d ", i, j, k, depth - i - j - k);
							combs++;
						}
					}
				}
			}
		}
	
		for(int i = depth-4; i>=(depth+4)/5; i--) {
			for(int j = depth-i-3; j>=(depth-i+3)/4; j--) {
				if(i>=j) {
					for(int k = depth-i-j-2; k>=(depth-i-j+2)/3; k--) {
						if(j>=k) {
							for(int l = depth-i-j-k-1; l>=(depth-i-j-k+1)/2; l--) {
								if(k>=l) {
									//~ printf("%d%d%d%d%d ", i, j, k, l, depth - i - j - k - l);
									combs++;
								}
							}
						}
					}
				}
			}
		}
		//~ printf("\n");
	}
	return combs;
}

int sig_compatator (const void* first, const void* second) {
	Signature * first_tmp = (Signature *) first;
	Signature * second_tmp = (Signature *) second;
	return memcmp((const void *)first_tmp->definition, (const void *)second_tmp->definition, 5);
}

struct Sig_tree {
	Signature * signatures;
	int sig_cnt;
	double best_price;
	int best_sig;
	double best_posib_price;
	int best_posib_sig;
};

bool sig_parent_child (Signature parent, Signature child) {
	if(child.level - parent.level != 1 ) return false;
	int diff = 0;
	for(int i = 0; i<5; i++) {
		int tmp = child.definition[i] - parent.definition[i];
		if(tmp < 0) tmp *= -1;
		diff += tmp;
	}
	return diff == 1;
}

double price_cnt (int disbalance, int level, unsigned cells) {
	if(level == 0) return (1+1/CONSTANT);
	return (1+(disbalance*1.0)/level)*(1+(cells - 4*level)/(CONSTANT*cells));
}

Sig_tree * init_sig_tree (int max_depth, int cells) {
	Sig_tree * retval = new Sig_tree();
	retval->sig_cnt = count_signatures(max_depth);
	retval->signatures = new Signature[retval->sig_cnt];
	generate_signatures(max_depth, retval->signatures, retval->sig_cnt);
	qsort((void *) retval->signatures, retval->sig_cnt, sizeof(Signature), sig_compatator);
	retval->best_price = price_cnt(0,0,cells);
	retval->best_posib_price = retval->best_price;
	retval->best_sig = 0;
	retval->best_posib_sig = 0;
	
	for(int i = 0; i<retval->sig_cnt; i++) {
		retval->signatures[i].price = price_cnt(retval->signatures[i].disbalance, retval->signatures[i].level, cells);
		if(retval->signatures[i].price < retval->best_posib_price) {
			retval->best_posib_price = retval->signatures[i].price;
			retval->best_posib_sig = i;
		}
		for(int j = i+1; j<retval->sig_cnt; j++) {
			if(sig_parent_child(retval->signatures[i], retval->signatures[j])) {
				retval->signatures[i].childs_cnt++;
				retval->signatures[j].parents_cnt++;
			}
		}
	}

	int * tmp_childs = new int [retval->sig_cnt];
	int * tmp_parents = new int [retval->sig_cnt];
	for(int i = 0; i<retval->sig_cnt; i++) {
		tmp_childs[i] = tmp_parents[i] = 0;
		if(retval->signatures[i].childs_cnt > 0) {
			retval->signatures[i].childs = new int [retval->signatures[i].childs_cnt];
		}
		if(retval->signatures[i].parents_cnt > 0) {
			retval->signatures[i].parents = new int [retval->signatures[i].parents_cnt];
		}
	}

	for(int i = 0; i<retval->sig_cnt; i++) {
		for(int j = i+1; j<retval->sig_cnt; j++) {
			if(sig_parent_child(retval->signatures[i], retval->signatures[j])) {
				retval->signatures[i].childs[tmp_childs[i]] = j;
				retval->signatures[j].parents[tmp_parents[j]] = i;
				tmp_parents[j]++;
				tmp_childs[i]++;
			}
		}
	}

	delete [] tmp_childs;
	delete [] tmp_parents;

	return retval;
}

void mark_node(Sig_tree * tree, int node_id) {
	if(tree->signatures[node_id].corupted == true) return;
	tree->signatures[node_id].corupted = true;
	for(int i = 0; i < tree->signatures[node_id].childs_cnt; i++) {
		if(tree->signatures[tree->signatures[node_id].childs[i]].corupted == false) {
			tree->signatures[node_id].corupted = false;
			return;
		}
	}
	if(tree->signatures[node_id].price < tree->best_price) {
		tree->signatures[node_id].corupted = false;
	}
}

void mark_nodes(Sig_tree * tree) {
	for(int i = tree->sig_cnt-1; i>=0; i--) {
		mark_node(tree, i);
	}
}

void del_sig_tree (Sig_tree * tree) {
	for(int i = 0; i<tree->sig_cnt; i++) {
			delete [] tree->signatures[i].childs;
			delete [] tree->signatures[i].parents;
	}
	delete [] tree->signatures;
	delete tree;
}

bool is_corupter (Sig_tree * tree, unsigned char definition []) {
	Signature tmp;
	tmp.level = 0;
	for(int i = 0; i<5; i++) {
		tmp.level += tmp.definition[i];
		tmp.definition[i] = definition[i];
	}
	Signature * sig = (Signature *) bsearch((void *) &tmp, (void *) tree->signatures, tree->sig_cnt, sizeof(Signature), sig_compatator);
	if(sig != NULL) {
		return sig->corupted;
	}
	return false;
}

struct Tetromino_block {
	char type;
	unsigned int positions[4]; 
};

struct Cell_to_tetro {
	unsigned int * tetronimos;
	unsigned int size;
	unsigned int filled;
};

unsigned count_tetromino_positions (unsigned width, unsigned height) {
	if(width < 3 || height < 3 ) {
		return 0;
	} else {
		unsigned retval = (width - 3) * height; // I horizontal
		retval += width * (height - 3); // I vertical
		retval += (width - 1) * (height - 1); // O
		retval += 2 * ( (width - 2) * (height - 1) ); // T horizontal
		retval += 2 * ( (width - 1) * (height - 2) ); // T vertical
		retval += 2 * ( (width - 2) * (height - 1) ); // S and Z horizontal
		retval += 2 * ( (width - 1) * (height - 2) ); // S and Z vertical
		retval += 4 * ( (width - 2) * (height - 1) ); // L and J horizontal
		retval += 4 * ( (width - 1) * (height - 2) ); // L and J vertical
		return retval;
	}
}

/*
 * position 0: ->IIII
 */
bool fit_I_horizontal(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+3 < width) {
		free_block.type = 'I';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+2+h*width;
		free_block.positions[3] = w+3+h*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->I
 *               I
 *               I
 *               I
 */
bool fit_I_vertical(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(h+3 < height) {
		free_block.type = 'I';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+(h+2)*width;
		free_block.positions[3] = w+(h+3)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->OO
 *               OO
 */
bool fit_O(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+1 < width && h+1 < height) {
		free_block.type = 'O';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+(h+1)*width;
		free_block.positions[3] = w+1+(h+1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->TTT
 *                T
 */
bool fit_T_horizontal_1(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+2 < width && h+1 < height) {
		free_block.type = 'T';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+2+h*width;
		free_block.positions[3] = w+1+(h+1)*width;
		return true;
	}
	return false;
}

/*
 *                T
 * position 0: ->TTT
 */
bool fit_T_horizontal_2(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	//~ if(w+2 < width && h > 0 && h < height) {
	if(w+2 < width && h > 0) {
		free_block.type = 'T';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+2+h*width;
		free_block.positions[3] = w+1+(h-1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->T
 *               TT
 *               T
 */
bool fit_T_vertical_1(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+1 < width && h+2 < height) {
		free_block.type = 'T';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+(h+2)*width;
		free_block.positions[3] = w+1+(h+1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->T
 *              TT
 *               T
 */
bool fit_T_vertical_2(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	//~ if(w>0 && w < width && h+2 < height) {
	if(w>0 && h+2 < height) {
		free_block.type = 'T';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+(h+2)*width;
		free_block.positions[3] = w-1+(h+1)*width;
		return true;
	}
	return false;
}

/*
 *                 L
 * position 0: ->LLL
 */
bool fit_L_horizontal_1(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	//~ if(w+2 < width && h>0 && h < height) {
	if(w+2 < width && h>0) {
		free_block.type = 'L';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+2+h*width;
		free_block.positions[3] = w+2+(h-1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->LLL
 *               L
 */
bool fit_L_horizontal_2(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+2 < width && h+1<height) {
		free_block.type = 'L';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+2+h*width;
		free_block.positions[3] = w+(h+1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->L
 *               L
 *               LL
 */
bool fit_L_vertical_1(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+1 < width && h+2<height) {
		free_block.type = 'L';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+(h+2)*width;
		free_block.positions[3] = w+1+(h+2)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->LL
 *                L
 *                L
 */
bool fit_L_vertical_2(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+1 < width && h+2<height) {
		free_block.type = 'L';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+1+(h+1)*width;
		free_block.positions[3] = w+1+(h+2)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->J 
 *               JJJ
 */
bool fit_J_horizontal_1(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+2 < width && h+1<height) {
		free_block.type = 'J';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+1+(h+1)*width;
		free_block.positions[3] = w+2+(h+1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->JJJ
 *                 J
 */
bool fit_J_horizontal_2(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+2 < width && h+1<height) {
		free_block.type = 'J';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+2+h*width;
		free_block.positions[3] = w+2+(h+1)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->J
 *               J
 *              JJ
 */
bool fit_J_vertical_1(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	//~ if(w > 0 && w < width && h+2<height) {
	if(w > 0 && h+2<height) {
		free_block.type = 'J';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+(h+2)*width;
		free_block.positions[3] = w-1+(h+2)*width;
		return true;
	}
	return false;
}

/*
 * position 0: ->JJ
 *               J
 *               J
 */
bool fit_J_vertical_2(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+1 < width && h+2<height) {
		free_block.type = 'J';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+(h+2)*width;
		free_block.positions[3] = w+1+h*width;
		return true;
	}
	return false;
}

/*
 *                SS
 * position 0: ->SS
 *
 */
bool fit_S_horizontal(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	//~ if(w+2 < width && h>0 && h<height) {
	if(w+2 < width && h>0) {
		free_block.type = 'S';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+1+(h-1)*width;
		free_block.positions[3] = w+2+(h-1)*width; 
		return true;
	}
	return false;
}

/*
 * position 0: ->S
 *               SS
 *                S
 */
bool fit_S_vertical(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+1 < width && h+2<height) {
		free_block.type = 'S';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w+1+(h+1)*width;
		free_block.positions[3] = w+1+(h+2)*width;
		return true;
	}
	return false;
}
/*
 * position 0: ->ZZ
 *                ZZ
 */
bool fit_Z_horizontal(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	if(w+2 < width && h+1<height) {
		free_block.type = 'Z';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+1+h*width;
		free_block.positions[2] = w+1+(h+1)*width;
		free_block.positions[3] = w+2+(h+1)*width; 
		return true;
	}
	return false;
}

/*
 * position 0: ->Z
 *              ZZ
 *              Z
 */
bool fit_Z_vertical(unsigned w, unsigned h, unsigned width, unsigned height, Tetromino_block & free_block) {
	//~ if(w>0 && w < width && h+2<height) {
	if(w>0 && h+2<height) {
		free_block.type = 'Z';
		free_block.positions[0] = w+h*width;
		free_block.positions[1] = w+(h+1)*width;
		free_block.positions[2] = w-1+(h+1)*width;
		free_block.positions[3] = w-1+(h+2)*width;
		return true;
	}
	return false;
}


Tetromino_block * generate_blocks (unsigned width, unsigned height, int seed, unsigned & block_count) {
	block_count = count_tetromino_positions(width, height);
	Tetromino_block * retval = new Tetromino_block[block_count];
	unsigned first_free = 0;

	for (unsigned h = 0; h<height; h++) {
		for (unsigned w = 0; w<width; w++) {
			if (fit_I_horizontal(w, h, width, height, retval[first_free])) {
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_I_vertical(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_O(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_T_horizontal_1(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_T_horizontal_2(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_T_vertical_1(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_T_vertical_2(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_L_horizontal_1(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_L_horizontal_2(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_L_vertical_1(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_L_vertical_2(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_J_horizontal_1(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_J_horizontal_2(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_J_vertical_1(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_J_vertical_2(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_S_horizontal(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_S_vertical(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_Z_horizontal(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
			
			if (fit_Z_vertical(w, h, width, height, retval[first_free])){
				//~ printf("%c: %d, %d, %d, %d\n", retval[first_free].type, retval[first_free].positions[0], retval[first_free].positions[1], retval[first_free].positions[2], retval[first_free].positions[3]);
				first_free++;
			}
		}
	}
	srand(seed);
	for (unsigned i = 0; i < block_count - 1; i++) {
		unsigned j = i + (rand() % (block_count-i));
		if(j!=i) {
			Tetromino_block tmp = retval[i];
			retval[i] = retval[j];
			retval[j] = tmp;
		}
	}

	//~ printf("%d\n", first_free);
	return retval;
}

unsigned * count_colisions (Tetromino_block * all_blocks, unsigned block_count, unsigned cells) {
	unsigned * colisions = new unsigned [cells];
	
	for(unsigned i = 0; i<cells; i++) {
		colisions[i] = 0;
	}
	
	for(unsigned i = 0; i<block_count; i++) {
		colisions[all_blocks[i].positions[0]]++;
		colisions[all_blocks[i].positions[1]]++;
		colisions[all_blocks[i].positions[2]]++;
		colisions[all_blocks[i].positions[3]]++;
	}
	
	return colisions;
}

Cell_to_tetro * join_cell_to_tetro (Tetromino_block * all_blocks, unsigned block_count, unsigned cells, unsigned * colisions) {
	Cell_to_tetro * cell_to_tetro = new Cell_to_tetro [cells];
	
	for(unsigned i = 0; i<cells; i++) {
		cell_to_tetro[i].tetronimos = new unsigned[colisions[i]];
		cell_to_tetro[i].size = colisions[i];
		cell_to_tetro[i].filled = 0;
	}
	
	for(unsigned i = 0; i<block_count; i++) {
		unsigned index = cell_to_tetro[all_blocks[i].positions[0]].filled;
		cell_to_tetro[all_blocks[i].positions[0]].tetronimos[index] = i;
		cell_to_tetro[all_blocks[i].positions[0]].filled++;
		
		index = cell_to_tetro[all_blocks[i].positions[1]].filled;
		cell_to_tetro[all_blocks[i].positions[1]].tetronimos[index] = i;
		cell_to_tetro[all_blocks[i].positions[1]].filled++;
		
		index = cell_to_tetro[all_blocks[i].positions[2]].filled;
		cell_to_tetro[all_blocks[i].positions[2]].tetronimos[index] = i;
		cell_to_tetro[all_blocks[i].positions[2]].filled++;
		
		index = cell_to_tetro[all_blocks[i].positions[3]].filled;
		cell_to_tetro[all_blocks[i].positions[3]].tetronimos[index] = i;
		cell_to_tetro[all_blocks[i].positions[3]].filled++;
	}
	return cell_to_tetro;
}

struct Problem {
	Tetromino_block * all_blocks;
	unsigned block_count;
	unsigned width;
	unsigned height;
	unsigned cells;

	int proc_cnt, proc_id;

	unsigned * colisions;
	Cell_to_tetro * cell_to_tetro;

	Sig_tree * tree;

	unsigned * used_blocks;
	unsigned used_blocks_cnt;

	bool * active_blocks;

	unsigned * used_cells;
	unsigned used_cells_cnt;

	bool * active_cells;

	double best_solution_price;
	int best_solution_src;
	double best_possible_price;
	unsigned * best_solution;
	unsigned best_blocks_cnt;
};

double cnt_best_possib_price (unsigned cells) {
	//~ return 1*(1+(cells%(TETROMINO_SIZE*TETROMINO_TYPES))/(1.0*cells)); 
	return 1.0;
}

double cnt_solution_price (unsigned cells, unsigned * solution, unsigned solution_blocks_cnt, Tetromino_block * all_blocks) {
	unsigned disbalance = 0;
	unsigned minimal_cnt = cells;
	unsigned types_cnts [TETROMINO_TYPES];

	for (unsigned i = 0; i<TETROMINO_TYPES; i++) {
		types_cnts[i] = 0;
	}

	for (unsigned i = 0; i<solution_blocks_cnt; i++) {
		char block_type = all_blocks[solution[i]].type;
		switch(block_type) {
			case 'I': types_cnts[I_INDEX]++;
				break;
			case 'O': types_cnts[O_INDEX]++;
				break;
			case 'T': types_cnts[T_INDEX]++;
				break;
			case 'L': types_cnts[L_INDEX]++;
				break;
			case 'J': types_cnts[J_INDEX]++;
				break;
			case 'S': types_cnts[S_INDEX]++;
				break;
			case 'Z': types_cnts[Z_INDEX]++;
				break;
		}
	}
	
	for (unsigned i = 0; i<TETROMINO_TYPES; i++) {
		if(types_cnts[i] < minimal_cnt) minimal_cnt = types_cnts[i];
	}
	for (unsigned i = 0; i<TETROMINO_TYPES; i++) {
		disbalance += types_cnts[i]-minimal_cnt;
	}
	return (1+(disbalance*1.0)/solution_blocks_cnt)*(1+(cells-solution_blocks_cnt*TETROMINO_SIZE)/(CONSTANT*cells));
}

Problem * init_problem (Tetromino_block * all_blocks, unsigned block_count, unsigned width, unsigned height, int proc_cnt, int proc_id) {
	Problem * retval = new Problem;
	retval->all_blocks = all_blocks;
	retval->block_count = block_count;
	retval->width = width;
	retval->height = height;
	retval->proc_cnt = proc_cnt;
	retval->proc_id = proc_id;
	retval->best_solution_src = retval->proc_id;

	retval->cells = retval->width*retval->height;
	retval->colisions = count_colisions(retval->all_blocks, retval->block_count, retval->cells);
	retval->cell_to_tetro = join_cell_to_tetro(retval->all_blocks, retval->block_count, retval->cells, retval->colisions);

	retval->used_blocks = new unsigned[retval->block_count];
	retval->used_blocks_cnt = 0;
	retval->active_blocks = new bool[retval->block_count];
	retval->used_cells = new unsigned[retval->cells];
	retval->used_cells_cnt = 0;
	retval->active_cells = new bool[retval->cells];

	for(unsigned i = 0; i<retval->block_count; i++) {
		retval->active_blocks[i] = true;
	}
	for(unsigned i = 0; i<retval->cells; i++) {
		retval->active_cells[i] = true;
	}

	retval->best_solution = new unsigned [retval->cells/TETROMINO_SIZE];
	retval->best_blocks_cnt = 0;
	retval->best_possible_price = cnt_best_possib_price(retval->cells);
	retval->best_solution_price = 2;

	retval->tree = init_sig_tree(retval->cells/TETROMINO_SIZE, retval->cells);

	return retval;
}

void delete_problem (Problem * problem) {
	del_sig_tree(problem->tree);
	delete [] problem->used_blocks;
	delete [] problem->active_blocks;
	delete [] problem->used_cells;
	delete [] problem->active_cells;
	for(unsigned i = 0; i<problem->cells; i++) {
		delete [] problem->cell_to_tetro[i].tetronimos;
	}
	delete [] problem->cell_to_tetro;
	delete [] problem->colisions;
	delete [] problem->best_solution;
	delete problem;
}

void print_best_solution (Problem * problem) {
	printf("------ processor %d out of %d processors ------\n", problem->proc_id, problem->proc_cnt);
	printf("blocks: %d , price: %10f, best posible price: %10f\n", problem->best_blocks_cnt, problem->best_solution_price, problem->tree->best_posib_price);
	printf("[\n");
	char * matrix = new char [problem->cells];
	for(unsigned i = 0; i<problem->cells; i++) {
		matrix[i] = ' ';
	}
	for(unsigned i = 0; i<problem->best_blocks_cnt; i++) {
		unsigned block = problem->best_solution[i];
		printf("\t(%c", problem->all_blocks[block].type);
		printf(", %d",    problem->all_blocks[block].positions[0]);
		printf(", %d",    problem->all_blocks[block].positions[1]);
		printf(", %d",    problem->all_blocks[block].positions[2]);
		printf(", %d)\n", problem->all_blocks[block].positions[3]);
		matrix[problem->all_blocks[block].positions[0]] = problem->all_blocks[block].type;
		matrix[problem->all_blocks[block].positions[1]] = problem->all_blocks[block].type;
		matrix[problem->all_blocks[block].positions[2]] = problem->all_blocks[block].type;
		matrix[problem->all_blocks[block].positions[3]] = problem->all_blocks[block].type;
	}
	printf("]\n");
	for(unsigned w = 0; w<2*problem->width+3; w++) {
		printf("#");
	}
	printf("\n");
	for(int h = problem->height - 1; h>=0; h--) {
		printf("# ");
		for(unsigned w = 0; w<problem->width; w++) {
			printf("%c ", matrix[h*problem->width+w]);
			//~ printf("%2d ", h*problem->width+w);
		}
		printf("#\n");
	}
	for(unsigned w = 0; w<2*problem->width+3; w++) {
		printf("#");
	}
	printf("\n");
	delete [] matrix;

}

bool pointless_sol(unsigned * solution, unsigned solution_blocks_cnt, Problem * problem) {
	unsigned char types_cnts [TETROMINO_TYPES];

	for (unsigned i = 0; i<TETROMINO_TYPES; i++) {
		types_cnts[i] = 0;
	}

	for (unsigned i = 0; i<solution_blocks_cnt; i++) {
		char block_type = problem->all_blocks[solution[i]].type;
		switch(block_type) {
			case 'I': types_cnts[I_INDEX]++;
				break;
			case 'O': types_cnts[O_INDEX]++;
				break;
			case 'T': types_cnts[T_INDEX]++;
				break;
			case 'L': types_cnts[L_INDEX]++;
				break;
			case 'J': types_cnts[J_INDEX]++;
				break;
			case 'S': types_cnts[S_INDEX]++;
				break;
			case 'Z': types_cnts[Z_INDEX]++;
				break;
		}
	}
	for (unsigned i = 0; i<TETROMINO_TYPES; i++) {
		unsigned max = i;
		for (unsigned j = i+1; j<TETROMINO_TYPES; j++) {
			if(types_cnts[j] > types_cnts[max]) {
				max = j;
			}
		}
		if(max!=i) {
			char tmp = types_cnts[i];
			types_cnts[i] = types_cnts[max];
			types_cnts[max] = tmp;
		}
	}

	return is_corupter(problem->tree, types_cnts);
}

void solve (Problem * problem, unsigned * parcial_solution, unsigned level, unsigned last_used_block = 0) {
	unsigned tested_blocks = 0;
	MPI_Status status;
	int flag = 0;
	unsigned split_level = (problem->cells/TETROMINO_SIZE)*SPLIT_LEVEL;
	for(unsigned i = last_used_block; i<problem->block_count; i++) {
		if(problem->best_solution_price < EPSILON*problem->tree->best_posib_price) return;
		if(problem->active_blocks[i]) {
			//~ printf("added: %d\n", i);
			problem->active_blocks[i] = false;
			problem->used_blocks[problem->used_blocks_cnt++] = i;
			tested_blocks++;
			if(level != split_level || 
				(level == split_level && tested_blocks%problem->proc_cnt == (unsigned) problem->proc_id)) {
				parcial_solution[level] = i;
				
				//STAR - process information from other processes
				if(tested_blocks%10 == 9) {
					bool change = false;
					while(true) {
						MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
						if (flag) {
							/* eceving message by blocking receive */
							double message;
							MPI_Recv(&message, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
							printf ("proc %d recieved %f from %d\n",problem->proc_id , message, status.MPI_SOURCE);
							if(message < problem->best_solution_price) {
								problem->best_solution_price = message;
								problem->tree->best_price = message;
								problem->best_solution_src = status.MPI_SOURCE;
								change = true;
							}
						} else break;
					}
					if(change == true) {
						mark_nodes(problem->tree);
					}
				}
				//END - process information from other processes
				
				//~ if(level < ((problem->cells)/TETROMINO_SIZE)-9) {
				//~ if(level < 4) {
					//~ printf("L:%02d, working on:%d of (%d, used %d)\n", level, tested_blocks, problem->block_count, problem->used_blocks_cnt);
				//~ }
				
				if(pointless_sol(parcial_solution, level+1, problem) == false ) {
					
					unsigned blocked_blocks = 0;
					
					double price = cnt_solution_price(problem->cells, parcial_solution, level+1, problem->all_blocks);
					if(problem->best_solution_price > price) {
						problem->best_solution_price = price;
						problem->best_blocks_cnt = level+1;
						for(unsigned j = 0; j<=level; j++) {
							problem->best_solution[j] = parcial_solution[j];
						}
						problem->tree->best_price = price;
						mark_nodes(problem->tree);
						print_best_solution(problem);
						
						//START - Send solution proce to other processes
						problem->best_solution_src = problem->proc_id;
						for(int i = 0; i < problem->proc_cnt; i++) {
							if(i!=problem->proc_id) {
								MPI_Send(&price, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
							}
						}
						//END - Send solution proce to other processes
					}
					
					for(unsigned j = 0; j<TETROMINO_SIZE; j++) {
						unsigned cell = problem->all_blocks[i].positions[j];
						problem->active_cells[cell] = false;
						problem->used_cells[problem->used_cells_cnt++] = cell;
						for(unsigned k = 0; k<problem->cell_to_tetro[cell].size; k++) {
							unsigned block = problem->cell_to_tetro[cell].tetronimos[k];
							if (problem->active_blocks[block]) {
								problem->active_blocks[block] = false;
								problem->used_blocks[problem->used_blocks_cnt++] = block;
								blocked_blocks++;
							}
						}
					}
					
					if(problem->block_count > problem->used_blocks_cnt) {
						solve(problem, parcial_solution, level+1, i);
					}
					
					while(blocked_blocks--){
						unsigned block = problem->used_blocks[problem->used_blocks_cnt-1];
						problem->used_blocks_cnt--;
						problem->active_blocks[block] = true;
					}
					for(unsigned j = 0; j<TETROMINO_SIZE; j++) {
						unsigned cell = problem->used_cells[problem->used_cells_cnt-1];
						problem->used_cells_cnt--;
						problem->active_cells[cell] = true;
					}
				}
			}
		}
	}
	while(tested_blocks--) {
		unsigned block = problem->used_blocks[problem->used_blocks_cnt-1];
		problem->used_blocks_cnt--;
		problem->active_blocks[block] = true;
	}
}

int main(int argc, char **argv)
{
	unsigned block_count;

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	int proc_cnt;
	MPI_Comm_size(MPI_COMM_WORLD, &proc_cnt);

	// Get the rank of the process
	int proc_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

	//~ printf("Hello world from processor %d out of %d processors\n", proc_id, proc_cnt);

	int width, height, seed;
	if(argc < 4) {
		printf("run program: %s WIDTH HEIGHT SEED\n", argv[0]);
		return 1;
	}
	//~ printf("enter width: ");
	//~ scanf("%d", &width);
	//~ printf("enter height: ");
	//~ scanf("%d", &height);
	//~ printf("enter seed: ");
	//~ scanf("%d", &seed);
	
	sscanf(argv[1], "%d", &width);
	sscanf(argv[2], "%d", &height);
	sscanf(argv[3], "%d", &seed);
	
	//~ Tetromino_block * all_blocks = generate_blocks(WIDTH, HEIGHT, SEED, block_count);
	Tetromino_block * all_blocks = generate_blocks(width, height, seed, block_count);
	//~ Problem * problem = init_problem(all_blocks, block_count, WIDTH, HEIGHT, proc_cnt, proc_id);
	Problem * problem = init_problem(all_blocks, block_count, width, height, proc_cnt, proc_id);
	unsigned * parcial_solution = new unsigned[problem->cells/TETROMINO_SIZE];
	solve(problem, parcial_solution, 0);
	int sig_index = problem->tree->best_posib_sig;
	if(problem->best_solution_src == problem->proc_id) {
		usleep(100);
		printf("\n----- END ----- END ----- END ----- END ----- END ----- END ----- END -----\n");
		printf("width: %d, height %d, seed %d\n", width, height, seed);
		printf("best posible price %f, disb: %d, level %d\n", problem->tree->best_posib_price, problem->tree->signatures[sig_index].disbalance, problem->tree->signatures[sig_index].level);
		print_best_solution(problem);
	}

	delete_problem(problem);
	delete [] parcial_solution;
	delete [] all_blocks;

	// Finalize the MPI environment.
	MPI_Finalize();

	return 0;
}

