//  PROJECT IDENTIFIER: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F
//  main.cpp
//  project3
//
//  Created by Joe Adams on 9/28/20.
//  Copyright © 2020 Joe Adams. All rights reserved.
//

#include <iostream>
#include "P2random.h"
#include <getopt.h>
#include <vector>
#include <deque>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;
#include "xcode_redirect.hpp"
struct tile{
    int num_rubble;
    bool discovered;
    bool pathed;
};
struct pq_tile{
    int num_rubble;
    size_t row;
    size_t col;
};
struct pq_compare{
    bool operator()(const pq_tile& lhs, const pq_tile& rhs){
        if((lhs.num_rubble == rhs.num_rubble) && (lhs.col == rhs.col)){
            return lhs.row > rhs.row;
        }
        else if(lhs.num_rubble == rhs.num_rubble){
            return lhs.col > rhs.col;
        }
        else{
            return lhs.num_rubble > rhs.num_rubble;
        }
    }//bool
};
struct pq_compare_oppo{
    bool operator()(const pq_tile& lhs, const pq_tile& rhs){
        if((lhs.num_rubble == rhs.num_rubble) && (lhs.col == rhs.col)){
            return lhs.row < rhs.row;
        }
        else if(lhs.num_rubble == rhs.num_rubble){
            return lhs.col < rhs.col;
        }
        else{
            return lhs.num_rubble < rhs.num_rubble;
        }
    }//bool
};
class MazeRunner{
private:
    vector<vector<struct tile>> map;
    priority_queue<pq_tile, vector<pq_tile>, pq_compare> primary_pq;
    priority_queue<pq_tile, vector<pq_tile>, pq_compare> tnt_pq;
    deque<double> medians;
    vector<pq_tile> stats_output;
    string mode;
    int stat_arg;
    size_t r;
    size_t c;
    size_t tiles_cleared;
    size_t rubble_cleared;
    uint32_t map_sz;
    bool stats;
    bool median;
    bool verbose;
    //private functions
    struct pq_tile create_tile(int in_rubble, size_t in_row, size_t in_col){
        struct pq_tile new_tile;
        new_tile.num_rubble = in_rubble;
        new_tile.col = in_col;
        new_tile.row = in_row;
        return new_tile;
    }
    bool is_border(const struct pq_tile& in_tile){
        if(((in_tile.col + 1 == map_sz) or (in_tile.col == 0) or (in_tile.row + 1 == map_sz) or (in_tile.row == 0))){
            return true;
        }//if
        else{
            return false;
        }//else
    }//is_border
    bool is_border2(const struct tile& in_tile, size_t row, size_t col){
        if(((col + 1 == map_sz) or (col == 0) or (row + 1 == map_sz) or (row == 0)) && in_tile.num_rubble == 0){
            return true;
        }//if
        else{
            return false;
        }//else
    }//is_border2
    void add(){
        if((r + 1 != map_sz) && (map[r+1][c].discovered != true)){
            if((map[r+1][c].num_rubble == -1) && (map[r][c].num_rubble == -1)){
                
            }else{
            primary_pq.push(create_tile(map[r+1][c].num_rubble, r+1, c));
            map[r+1][c].discovered = true;
            }
        }
        if((r != 0) && (map[r-1][c].discovered != true)){
            if((map[r-1][c].num_rubble == -1) && (map[r][c].num_rubble == -1)){
                
            }else{
            primary_pq.push(create_tile(map[r-1][c].num_rubble, r-1, c));
            map[r-1][c].discovered = true;
            }
        }
        if((c + 1 != map_sz) && (map[r][c+1].discovered != true)){
            if((map[r][c+1].num_rubble == -1) && (map[r][c].num_rubble == -1)){
                
            }else{
            primary_pq.push(create_tile(map[r][c+1].num_rubble, r, c+1));
            map[r][c+1].discovered = true;
            }
        }
        if((c != 0) && (map[r][c-1].discovered != true)){
            if((map[r][c-1].num_rubble == -1) && (map[r][c].num_rubble == -1)){
                
            }else{
            primary_pq.push(create_tile(map[r][c-1].num_rubble, r, c-1));
            map[r][c-1].discovered = true;
            }
        }
    }//add
    void add_tnt(){
        if((r + 1 != map_sz) && (map[r+1][c].pathed != true)){
            tnt_pq.push(create_tile(map[r+1][c].num_rubble, r+1, c));
            map[r+1][c].pathed=true;
            if(map[r+1][c].num_rubble >=0){
                primary_pq.push(create_tile(0, r+1, c));
            }
        }
        if((r != 0) && (map[r-1][c].pathed != true) ){
            tnt_pq.push(create_tile(map[r-1][c].num_rubble, r-1, c));
            map[r-1][c].pathed=true;
            if(map[r-1][c].num_rubble >=0){
                primary_pq.push(create_tile(0, r-1, c));
            }
        }
        if((c + 1 != map_sz) && (map[r][c+1].pathed != true) ){
            tnt_pq.push(create_tile(map[r][c+1].num_rubble, r, c+1));
            map[r][c+1].pathed=true;
            if(map[r][c+1].num_rubble >=0){
                primary_pq.push(create_tile(0, r, c+1));
            }
        }
        if((c != 0) && (map[r][c-1].pathed != true) ){
            tnt_pq.push(create_tile(map[r][c-1].num_rubble, r, c-1));
            map[r][c-1].pathed=true;
            if(map[r][c-1].num_rubble >=0){
                primary_pq.push(create_tile(0, r, c-1));
            }
        }
    }//add_tnt
    void explode_tnt(){
        if(!tnt_pq.empty()){
        while((tnt_pq.top().num_rubble == -1) && !tnt_pq.empty()){
            if(verbose){
                cout << "TNT explosion at " << "[" << tnt_pq.top().row << "," << tnt_pq.top().col << "]!\n";
            }
            if(stats){
                stats_output.push_back(tnt_pq.top());
            }
            r=tnt_pq.top().row;
            c=tnt_pq.top().col;
            map[r][c].num_rubble = 0;
            map[r][c].discovered = true;
            tnt_pq.pop();
            add_tnt();
        }
        }
        while(!tnt_pq.empty()){
            if(verbose && tnt_pq.top().num_rubble != 0){
                cout << "Cleared by TNT: " << tnt_pq.top().num_rubble << " at " << "[" << tnt_pq.top().row << "," << tnt_pq.top().col << "]\n";
            }
            if((median) && (map[tnt_pq.top().row][tnt_pq.top().col].num_rubble > 0)){
                if(medians.size() > 500){
                    medians.pop_back();
                    medians.pop_front();
                }
                medians.push_back(map[tnt_pq.top().row][tnt_pq.top().col].num_rubble);
                sort(medians.begin(), medians.end());
                if(medians.size() % 2 == 0){
                    cout << "Median difficulty of clearing rubble is: " << (medians[medians.size()/2] + medians[(medians.size()/2) -1])/2 << "\n";
                }
                else{
                    cout << "Median difficulty of clearing rubble is: " << medians[medians.size()/2] << "\n";
                }
            }
            if(stats and (map[tnt_pq.top().row][tnt_pq.top().col].num_rubble > 0)){
                stats_output.push_back(tnt_pq.top());
            }
            if(map[tnt_pq.top().row][tnt_pq.top().col].num_rubble!=0){
                tiles_cleared++;
            }
            r=tnt_pq.top().row;
            c=tnt_pq.top().col;
            rubble_cleared+=static_cast<size_t>(map[tnt_pq.top().row][tnt_pq.top().col].num_rubble);
            map[tnt_pq.top().row][tnt_pq.top().col].num_rubble = 0;
            primary_pq.push(create_tile(0, r, c));
            tnt_pq.pop();
        }
        if(is_border2(map[r][c], r, c) && primary_pq.empty()){
            return;
        }
        r=primary_pq.top().row;
        c=primary_pq.top().col;
    }//explode_tnt
    void clear(){
        if(map[r][c].pathed){
            primary_pq.pop();
            return;
        }
        if(map[r][c].num_rubble == -1){
            if(verbose){
                cout << "TNT explosion at " << "[" << r << "," << c << "]!\n";
            }
            if(stats){
                stats_output.push_back(primary_pq.top());
            }
            map[r][c].pathed = true;
            map[r][c].num_rubble = 0;
            primary_pq.pop();
            add_tnt();
            explode_tnt();
        }//if
        else{
            if((verbose) && (primary_pq.top().num_rubble > 0)) {
                cout << "Cleared: " << primary_pq.top().num_rubble << " at [" << primary_pq.top().row << "," << primary_pq.top().col << "]\n";
            }
            if((median) && (primary_pq.top().num_rubble > 0)){
                if(medians.size() > 500){
                    medians.pop_back();
                    medians.pop_front();
                }
                medians.push_back(primary_pq.top().num_rubble);
                sort(medians.begin(), medians.end());
                if(medians.size() % 2 == 0){
                    cout << "Median difficulty of clearing rubble is: " << ((medians[medians.size()/2] + medians[(medians.size()/2) -1]))/2 << "\n";
                }
                else{
                    cout << "Median difficulty of clearing rubble is: " << medians[medians.size()/2] << "\n";
                }
            }
            if(stats&& (primary_pq.top().num_rubble > 0)){
                stats_output.push_back(primary_pq.top());
            }
            rubble_cleared+=static_cast<size_t>(map[r][c].num_rubble);
            if(map[r][c].num_rubble!=0){
                tiles_cleared++;
            }
            map[r][c].num_rubble = 0;
            map[r][c].pathed = true;
            r=primary_pq.top().row;
            c=primary_pq.top().col;
            primary_pq.pop();
        }//else
    }//clear
public:
    MazeRunner(){
        map_sz = 0;
        r = 0;
        c = 0;
        stat_arg=0;
        tiles_cleared = 0;
        rubble_cleared = 0;
        stats = false;
        median = false;
        verbose = false;
    }
    void get_options(int argc, char * argv[]){
        opterr = true;
        int choice;
        int option_index = 0;
        struct option long_options[] = {
            {"stats", required_argument, nullptr,'s'},
            {"median", no_argument, nullptr,'m'},
            {"verbose", no_argument, nullptr,'v'},
            {"help", no_argument, nullptr, 'h'},
            { nullptr, 0,                 nullptr, '\0' }
        };
        while ((choice = getopt_long(argc, argv, "s:mvh", long_options, &option_index)) != -1) {
            switch (choice) {
                case 'h':
                    cerr << "You asked for help" << flush;
                    exit(0);
                case 's':
                    stats=true;
                    if(optarg[0] > 0){
                        stat_arg = stoi(optarg);
                    }
                    break;
                case 'm':
                    median = true;
                    break;
                case 'v':
                    verbose = true;
                    break;
                default:
                    cerr << "Error: invalid option" << flush;
                    exit(1);
            } // switch
        } // while
    }//get_options()
    void read_map(){
        stringstream ss;
        string info;
        cin>>mode;
        if(mode != "M" and mode != "R"){
            cerr << "Invalid input mode" << flush;
            exit(1);
        }
        cin.ignore();
        getline(cin, info);
        info = info.substr(6, info.size()-6);
        map_sz = static_cast<uint32_t>(stol(info));
        getline(cin, info);
        info = info.substr(7, info.size()-7);
        bool breaked = false;
        string builder, builder2;
        for(auto x : info){
            if(x == ' '){
                breaked = true;
            }
            else if(breaked){
                builder += x;
            }
            else{
                builder2 += x;
            }
        }
        c = static_cast<size_t>(stoi(builder));
        r = static_cast<size_t>(stoi(builder2));
        if(r > map_sz){
            cerr << "Invalid starting row" << flush;
            exit(1);
        }
        if(c > map_sz){
            cerr << "Invalid starting column" << flush;
            exit(1);
        }
        if(mode == "R"){
            getline(cin, info);
            info = info.substr(6, info.size()-6);
            uint32_t seed = static_cast<uint32_t>(stol(info));
            getline(cin, info);
            info = info.substr(12, info.size()-12);
            uint32_t max_rubble = static_cast<uint32_t>(stol(info));
            getline(cin, info);
            info = info.substr(5, info.size()-5);
            uint32_t tnt_chance = static_cast<uint32_t>(stol(info));
            P2random::PR_init(ss, map_sz, seed, max_rubble, tnt_chance);
        }//if
        istream &inputStream = (mode == "M") ? cin : ss;
        int temp;
        size_t i = 0;
        size_t j = 0;
     //   map.resize(map_sz,vector<struct tile>(map_sz));
        map.resize(map_sz);
        while(inputStream >> temp){
            struct tile tile;
            tile.num_rubble = temp;
            tile.discovered = false;
            tile.pathed = false;
            map[i].push_back(tile);
            j++;
            if(j == (map_sz)){
                ++i;
                j=0;
            }
        }//while
    }//read_map
    void escape(){
        primary_pq.push(create_tile(map[r][c].num_rubble, r, c));
        map[r][c].discovered = true;
        while(!is_border2(map[r][c],r,c)){
            clear();
            if(is_border2(map[r][c], r, c)){
                break;
            }
            add();
            r=primary_pq.top().row;
            c=primary_pq.top().col;
        }//while
        return;
    }//escape
    void print(){
        cout << "Cleared " << tiles_cleared << " tiles containing " << rubble_cleared << " rubble and escaped.\n";
        if(stats){
            cout << "First tiles cleared:\n";
            if(!stats_output.empty()){
                for(size_t i = 0; i < (static_cast<size_t>(stat_arg)); ++i){
                    if(i == stats_output.size()){
                        break;
                    }
                    if(stats_output[i].num_rubble > 0){
                        cout << stats_output[i].num_rubble << " at [" << stats_output[i].row << "," << stats_output[i].col << "]\n";
                    }
                    else{
                        cout <<"TNT at [" << stats_output[i].row << "," << stats_output[i].col << "]\n";
                    }
                }//for
            }//if
            cout << "Last tiles cleared:\n";
            if(!stats_output.empty()){
                for(int i = (int)(stats_output.size() - 1); ((int)stats_output.size())-i-1 < (static_cast<int>(stat_arg)); --i){
                    if(i == -1){
                        break;
                    }
                    if(stats_output[(size_t)i].num_rubble > 0){
                        cout << stats_output[(size_t)i].num_rubble << " at [" << stats_output[(size_t)i].row << "," <<  stats_output[(size_t)i].col << "]\n";
                    }
                    else{
                        cout <<"TNT at [" << stats_output[(size_t)i].row << "," << stats_output[(size_t)i].col << "]\n";
                    }
                }//for
            }//if
            priority_queue<pq_tile, vector<pq_tile>, pq_compare> easy;
            priority_queue<pq_tile, vector<pq_tile>, pq_compare_oppo> hard;
            for(size_t i = 0; i < stats_output.size(); ++i){
                easy.push(stats_output[i]);
                hard.push(stats_output[i]);
            }
            cout << "Easiest tiles cleared:\n";
            if(!stats_output.empty()){
                for(size_t i = 0; i< static_cast<size_t>(stat_arg) ; ++i){
                    if(i == stats_output.size()){
                        break;
                    }
                    if(easy.top().num_rubble > 0){
                        cout << easy.top().num_rubble << " at [" << easy.top().row << "," <<  easy.top().col << "]\n";
                    }
                    else{
                        cout <<"TNT at [" << easy.top().row << "," << easy.top().col << "]\n";
                    }
                    easy.pop();
                }//for
            }//if
            cout << "Hardest tiles cleared:\n";
            if(!stats_output.empty()){
                for(size_t i = 0; i< static_cast<size_t>(stat_arg) ; ++i){
                    if(i == stats_output.size()){
                        break;
                    }
                    if(hard.top().num_rubble > 0){
                        cout << hard.top().num_rubble << " at [" << hard.top().row << "," <<  hard.top().col << "]\n";
                    }
                    else{
                        cout <<"TNT at [" << hard.top().row << "," << hard.top().col << "]\n";
                    }
                    hard.pop();
                }//for
            }//if
        }//stats if
        return;
    }//print
};//class
int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    cout << std::fixed << std::setprecision(2);
    MazeRunner m1;
    m1.get_options(argc, argv);
    m1.read_map();
    m1.escape();
    m1.print();
    return 0;
}
