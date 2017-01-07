// DataLink.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"			//comment this header if running in a non-windows environment
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include<list>
#include<map>
#include<algorithm>
#include<bitset>
#include<iterator>

using namespace std;

const int KNODES_SIZE = 4;		//Number of constant node pointers a link can hold
const int KLINKS_SIZE = 3;		//Number of constant link pointers a node can hold
const int DLEN_min = 32;
const int DLEN_max = 255;

int snd_call_counter = 0;		//in case of fragmentation- to increment F_seq field for each frame beginning from 32
int chck_seq = 0;				//to check F-seq field of each frame in case of reverse fragments in snd_f8r()

map<int, string> str_fragmnt_map;	//to maintain the framgment F_seq and corresponding msg fragment pair

map<string, int> m1;				//host map for node A
map<string, int> m2;				//host map for node B
map<string, int> m3;				//host map for node C
map<string, int> m4;				//host map for node D

class Node; //forward declaration of class Node

class Frame			//contains all fields and methods to construct an F8 frame
{
public:
	const string version = "F8";	//protocol name
	const int type = 1;				//default value
	const int ttl = 31;				//default value
	const int dLEN = 57;			//msg length in bytes
	const string src_addr = "196.168.2.1";		//ip address of source node
	const string dest_addr = "196.168.2.2";		//ip address of destination node
	const int prtcl = 1;			//default value
	int ident;						//will be set according to fragmentation done or not
	int seq;						//will be set according to fragmentation done or not

	string send_str = "CPSC 558 -- Adv Networking -- Project Part 1 -- Data Link";			//text string to be sent

	vector<bitset<8>> build_bitset_vector();		//function to construct F8 frame
	bitset<8> calculate_header_checksum(vector<bitset<8>> bitset_vec);		//function to calculate header checksum
};

//construct F8 frame and returns Frame vector
vector<bitset<8>> Frame::build_bitset_vector()
{
	bitset<8> b1, b2;
	b1 = version[0];
	b2 = version[1];
	stringstream ss(src_addr);
	vector<int> src_octets;
	string temp;
	while (getline(ss, temp, '.'))
		src_octets.push_back(atoi(temp.c_str()));
	unsigned int i, j, k, l;
	i = src_octets.at(0);
	j = src_octets.at(1);
	k = src_octets.at(2);
	l = src_octets.at(3);

	const std::bitset<8> frame_type(type), frame_ttl(ttl), frame_DLEN(dLEN), protocol(prtcl), /*f_ident(ident), f_seq(seq),*/ s1(i), s2(j), s3(k), s4(l), d4(2);
	bitset<8> type_ttl = (frame_type << 5) | frame_ttl;

	vector<bitset<8>> bitset_vec;
	bitset_vec.push_back(b1);
	bitset_vec.push_back(b2);
	bitset_vec.push_back(type_ttl);
	bitset_vec.push_back(frame_DLEN);
	bitset_vec.push_back(s1);
	bitset_vec.push_back(s2);
	bitset_vec.push_back(s3);
	bitset_vec.push_back(s4);
	bitset_vec.push_back(s1);
	bitset_vec.push_back(s2);
	bitset_vec.push_back(s3);
	bitset_vec.push_back(d4);
	bitset_vec.push_back(protocol);
	//bitset_vec.push_back(f_ident);
	//bitset_vec.push_back(f_seq);

	return bitset_vec;
}

//calculates header checksum of all bytes in frame
bitset<8> Frame::calculate_header_checksum(vector<bitset<8>> bit_vector)
{
	bitset<8> h_chksum;
	for (int i = 0; i < bit_vector.size(); i++)
		h_chksum ^= bit_vector[i];
	return h_chksum;
}

class Link
{
public:
	int link_id;
	int nodeCountLLA1;							//count to compare with K_NODES_SIZE					
												//list<Node*> node_ptr_list;					
	vector<Node*> node_ptr_list;				//array to attach node pointers to Link

	Link(string par_object_name);	//parameterized constructor for class Link
	~Link();									//destructor

	//Declaration of functions of class Link
	string name();
	int attach(Node* node_ptr);
	void contents();
	//string xmt(string input_str, Node* node_ptr);
	vector<bitset<8>> xmt(vector<bitset<8>> input_frm, Node* node_ptr);

private:
	string link_object_name;
};
//Link constructor
Link::Link(string par_object_name)
{
	link_object_name = par_object_name;
}
//Link destructor
Link::~Link()
{
}

class Node
{
public:
	//list<string> rcvd_string_list;		
	vector<string> rcvd_string_list;		//in case if list does not work
	vector<Link*> link_ptr_list;
	int node_id;
	

	int linkCountLLA1;
	Link* link;

	Node(string par_object_name, string par_node_address);			//parameterized constructor for class Node
	~Node();											//destructor

	//declarations of functions of class Node
	string name();
	int attach(Link* link_ptr);
	void contents();
	void rcv(vector<bitset<8>> input_frm);
	void rcv_f8(vector<bitset<8>> input_frm);
	void snd_f8(string input_str, int link_id, int bytes_count);
	void snd_f8r(string input_str, int link_id, int bytes_count);


private:
	string node_object_name;
	string node_address;
};
//Node constructor
Node::Node(string par_object_name, string par_node_address)
{
	node_object_name = par_object_name;
	node_address = par_node_address;
}
//Node destructor
Node::~Node()
{
}

// Return the Link object's name string (eg, assigned during object construction).
//Active line count = 1
string Link::name()
{
	return link_object_name;
}

//Install node pointer as a link member and return integer ID for that node pointer
//Active line count = 4
int Link::attach(Node* node_ptr)
{
	if (nodeCountLLA1 >= KNODES_SIZE)   return -1; //do nothing

	node_ptr_list.push_back(node_ptr);
	node_ptr->node_id = nodeCountLLA1;
	nodeCountLLA1++;
	return (node_ptr->node_id);
}

//Output each currently attached node's name, prefaced by its ID. 
//Active line count = 2
void Link::contents()
{
	for (int i = 0; i < node_ptr_list.size(); i++)
		cout << "Contents of Link: " << (node_ptr_list[i])->node_id << ": " << node_ptr_list[i]->name() << endl;
}

//Pass frame to the rcv() fcn of each of the other attached nodes (ie, skipping the caller). 
vector<bitset<8>> Link::xmt(vector<bitset<8>> input_frm, Node* node_ptr)
{
	//to check if the calling node attached to link or not
	if (find(node_ptr_list.begin(), node_ptr_list.end(), node_ptr) == node_ptr_list.end())
		cout<< "calling node is not attached"<<endl;
	else
	{
		vector<char> vec_char;	//char vector to extract message from frame;
		for (int i = 16; i < input_frm.size()-1; i++)	//to extract the string message from frame
		{
			vec_char.push_back(static_cast<char>(input_frm[i].to_ulong()));
		}
		string str_message(vec_char.begin(), vec_char.end());
		vector<Node*>::iterator node_iterator;
		for (node_iterator = this->node_ptr_list.begin(); node_iterator != this->node_ptr_list.end(); node_iterator++)
		{
			if (*node_iterator != node_ptr)
			{
				string str = "Link " + link_object_name + " xmtd: " + str_message;
				cout << str << endl<<endl;
				(*node_iterator)->rcv(input_frm);			//call to rcv() fcn of Link
				return input_frm;
			}
		}
	}
}

// Return the Link object's name string (eg, assigned during object construction).
//Active line count = 1
string Node::name()
{
	return node_object_name;
}

//Take Link pointer & install to the node, return integer ID for that link pointer.
//Active line count = 3;
int Node::attach(Link* link_ptr)
{
	if (linkCountLLA1 >= KLINKS_SIZE)	return -1;
	
	if (find(this->link_ptr_list.begin(), this->link_ptr_list.end(), link) == this->link_ptr_list.end())
	{	//cout << " link is not attached for ID : " << id_of_link << endl;
		link = link_ptr;
		link_ptr_list.push_back(link);
		/*linkCountLLA1++;
		return (link_ptr->get_link_id());*/

		link->link_id = linkCountLLA1;
		linkCountLLA1++;
	}
	else
	{
		link = link_ptr;
		link_ptr_list.push_back(link);
		link->link_id = linkCountLLA1+1;
		linkCountLLA1++;
	}
	return (link->link_id);
}

//Outputs each currently attached link pointer value in hex, prefaced by its ID.
//Active line count = 2;
void Node::contents()
{
	for (int i = 0; i < link->name().size(); i++)
		cout << "Contents of Node: " << link->link_id << " : " << link->name() << " in hex format: " << link->link_id << " : " << hex << (int)link->name()[i] << endl;
}

//check if first 2 bytes of frame are "F8", if yes, call rcv_f8() function
void Node::rcv(vector<bitset<8>> input_frm)
{
	bitset<8> b1('F'), b2('8');
	//to check whether first 2 bytes of frame are "F8"
	if (input_frm[0] == b1 && input_frm[1] == b2)
		this->rcv_f8(input_frm);
	else
		cout << "Error : Not F8 frame" << endl;
}

//extract message from F8 frame and display as output
void Node::rcv_f8(vector<bitset<8>> input_frm)
{
	string str;
	vector<char> vec_char;
	bitset<8> f_ident(32);		//to check if frame fragmented or not

	//to calculate and verify checksum at receiving side
	bitset<8> chck_h_chksum, chck_m_chksum;		//to verify header checksum and message checksum
	for (int i = 0; i < 15; i++)
		chck_h_chksum ^= input_frm[i];
	cout << "header checksum in bytes at receiver side : " << chck_h_chksum << endl << endl;

	if (chck_h_chksum == input_frm[15])			//check if sender side and receiver side header checksum are equal 
	{
		cout << "header checksum verified at receiver side" << endl<<endl;
		for (int i = 15; i < input_frm.size() - 1; i++)
			chck_m_chksum ^= input_frm[i];
		cout << "message checksum in bytes at receiver side : " << chck_m_chksum << endl << endl;
		if (chck_m_chksum == input_frm.back())		////check if sender side and receiver side message checksum are equal 
			cout << "message checksum verified at receiver side" << endl<<endl;
	}
	else
		cout << "Checksum wrong!!! Frame corrupted!! " << endl;
			
	//extract string message from frame
	for (int i = 16; i < input_frm.size()-1; i++)
		vec_char.push_back(static_cast<char>(input_frm[i].to_ulong()));

	string str_message(vec_char.begin(), vec_char.end());
	
	if (input_frm[13] == f_ident)		//check F_ident field of frame equals 32 i.e. no fragmentation
		cout << "Node " + node_object_name + " rcvd_f8 : " + str_message<<endl;	//display entire message
	else                                //fragmentation present
		str_fragmnt_map[static_cast<int>(input_frm[14].to_ulong())] = str_message;		//store string fragments
	if (str_fragmnt_map.size() == 5)
	{
		cout << "Message obtained after extracting fragments from each frame : " << endl;
		for (std::map<int, string>::iterator i = str_fragmnt_map.begin(); i != str_fragmnt_map.end(); i++)	//arrange frames in order
			cout<< i->second;
	}
	cout << endl;

	vector<int> vec_int;
	//extract destination ip address from frame
	for (int i = 8; i < 12; i++)
		vec_int.push_back(static_cast<int>(input_frm[i].to_ulong()));

	std::stringstream result;
	string dest_address;
	std::copy(vec_int.begin(), vec_int.end(), std::ostream_iterator<int>(result, "."));
	dest_address = result.str().substr(0, result.str().length() - 1);
	
	//check whether node's ip address is same as destination address in frame
	if(this->node_address == dest_address)
	{
		cout <<endl<< "this is the destination node" << endl;
	}
	else    //forward message to destination node
	{
		cout << "not dest node, forwarding to destination node " << endl;
		for (map<string, int>::iterator it = m2.begin(); it != m2.end(); ++it) {
			if (it->first == dest_address)
			{
				snd_f8(str_message, it->second, 74);
			}
		}
	}
}
// takes input string and wraps it in frame and passes to xmt() fcn of the link object
void Node::snd_f8(string input_str, int id_of_link, int bytes_count)
{
	int count = 0;
	bitset<8> sum_str_byte;
	Frame f;
	f.ident = 32;
	f.seq = 32;
	bitset<8> bt, f_ident(f.ident), f_seq(f.seq);
	vector<bitset<8>> vec = f.build_bitset_vector();
	vec.push_back(f_ident);
	vec.push_back(f_seq);

	if (f.ident == 32)	//check if fragmentation done or not
	{
		bt = f.calculate_header_checksum(vec);
		cout << "header checksum in bytes at sender side : " << bt << endl << endl;
		vec.push_back(bt);
		vector<char> bytes(input_str.begin(), input_str.end());		//convert input string message to char vector
		for (int i = 0; i < bytes.size(); i++)
		{
			bitset<8> str_byte = bytes[i];		//convert each char in string message to byte
			vec.push_back(bytes[i]);			//push each byte of message string to frame;
			count++;
			sum_str_byte ^= str_byte;
		}
		/*for (int i = 0; i < vec.size(); i++)		count++;*/
		bt ^= sum_str_byte;		//calculate message checksum
		cout << "message checksum in bytes at sender side : " << bt << endl << endl;
		vec.push_back(bt);		//push message checksum to frame
		//count++;
		//std::cout << "Total number of data bytes in frame :" << std::dec << count << endl;

		vector<Link*>::iterator link_iterator;
		for (link_iterator = this->link_ptr_list.begin(); link_iterator != this->link_ptr_list.end(); link_iterator++)
		{
			if (link->link_id == id_of_link) {
				cout << "Node " + this->node_object_name + " sent: " + input_str << endl<<endl;
				link->xmt(vec, this);		//call to xmt() fcn of Link
			}
		}
	}
	else if(f.ident > 32)			//message fragmented into frame size of 30 bytes each
	{
		if (f.seq == 32) //this is first fragment
		{
			bitset<8> d_len(13), ff_seq(f.seq+snd_call_counter), ff_ident(f.ident);
			vec[3] = d_len;		//set DLEN field n frame to 13 bytes
			vec[13] = ff_ident;	//set F_ident to 33 i.e. >32
			vec[14] = ff_seq;	//set F_seq to incremental seq no. begining 32;
			bt = f.calculate_header_checksum(vec);
			vec.push_back(bt);
			vector<char> bytes(input_str.begin(), input_str.end());
			for (int i = 0; i < bytes.size(); i++)
			{
				bitset<8> str_byte = bytes[i];		//convert each char in string message to byte
				vec.push_back(bytes[i]);			//push msg to frame
				count++;
				sum_str_byte ^= str_byte;
			}
			//for (int i = 0; i < vec.size(); i++)		count++;
			bt ^= sum_str_byte;		
			vec.push_back(bt);		//push message checksum to frame
			//count++;
			//std::cout << "Total number of data bytes in frame :" << std::dec << count << endl;
			snd_call_counter++;

			vector<Link*>::iterator link_iterator;
			for (link_iterator = this->link_ptr_list.begin(); link_iterator != this->link_ptr_list.end(); link_iterator++)
			{
				if (link->link_id == id_of_link) {
					cout << "Node " + this->node_object_name + " sent: " + input_str << endl;
					link->xmt(vec, this);		//call to xmt() fcn of Link
				}
			}
		}
	}
}

//reverses the fragments and sends each fragment in individual frame
void Node::snd_f8r(string input_str, int id_of_link, int bytes_count)
{
	int count = 0;
	bitset<8> sum_str_byte;
	Frame f;
	f.ident = 33;
	f.seq = 36;
	bitset<8> bt, f_ident(f.ident), f_seq(f.seq);
	vector<bitset<8>> vec = f.build_bitset_vector();
	vec.push_back(f_ident);
	vec.push_back(f_seq);

	if (f.ident > 32)
	{
		if (f.seq == 36) //this is first fragment
		{
			bitset<8> d_len(13), ff_seq(f.seq - snd_call_counter), ff_ident(f.ident);
			vec[3] = d_len;	//set DLEN field n frame to 13 bytes
			vec[13] = ff_ident;
			vec[14] = ff_seq;
			bt = f.calculate_header_checksum(vec);
			vec.push_back(bt);
			vector<char> bytes(input_str.begin(), input_str.end());
			for (int i = 0; i < bytes.size(); i++)
			{
				bitset<8> str_byte = bytes[i];		//convert each char in string message to byte
				vec.push_back(bytes[i]);
				count++;
				sum_str_byte ^= str_byte;
			}
			//for (int i = 0; i < vec.size(); i++)		count++;
			bt ^= sum_str_byte;		//message checksum
			vec.push_back(bt);
			//count++;
			//std::cout << "Total number of data bytes in frame :" << std::dec << count << endl;
			snd_call_counter++;

			vector<Link*>::iterator link_iterator;
			for (link_iterator = this->link_ptr_list.begin(); link_iterator != this->link_ptr_list.end(); link_iterator++)
			{
				if (link->link_id == id_of_link) {
					cout << "Node " + this->node_object_name + " sent_f8r: " + input_str << endl;
					link->xmt(vec, this);		//call to xmt() fcn of Link
				}
			}
		}
	}
}

int main()
{
	string send_str = "CPSC 558 -- Adv Networking -- Project Part 1 -- Data Link";			//Binary text string to be sent
	string receive_str = "01101";		//Binary text string to be received

										//Create two nodes and one link, by calling the Link and Node constructors.
	Link* pL1 = new Link("L1");
	Link* pL2 = new Link("L2");
	Link* pL3 = new Link("L3");

	Node* pA = new Node("A", "196.168.2.1");
	Node* pB = new Node("B", "196.168.2.2");
	Node* pC = new Node("C", "196.168.2.3");
	Node* pD = new Node("D", "196.168.2.6");

	//call attach() function of A, B, L.
	int nodeIdA = pL1->attach(pA);
	cout << "Attached " << pA->name() << " in link " << pL1->name() << " as : " << nodeIdA << endl;
	cout << endl;

	int nodeIdB1 = pL1->attach(pB);
	cout << "Attached " << pB->name() << " in link " << pL1->name() << " as : " << nodeIdB1 << endl;
	cout << endl;

	int linkIdL11 = pA->attach(pL1);
	cout << "Attached " << pL1->name() << " in node " << pA->name() << " as : " << linkIdL11 << endl;
	cout << endl;

	int linkIdL1 = pB->attach(pL1);
	cout << "Attached " << pL1->name() << " in node " << pB->name() << " as : " << linkIdL1 << endl;
	cout << endl;

	//
	int nodeIdB2 = pL2->attach(pB);
	cout << "Attached " << pB->name() << " in link " << pL2->name() << " as : " << nodeIdB2 << endl;
	cout << endl;

	int nodeIdC = pL2->attach(pC);
	cout << "Attached " << pC->name() << " in link " << pL2->name() << " as : " << nodeIdC << endl;
	cout << endl;

	int linkIdL2 = pB->attach(pL2);
	cout << "Attached " << pL2->name() << " in node " << pB->name() << " as : " << linkIdL2 << endl;
	cout << endl;

	int linkIdL22 = pC->attach(pL2);
	cout << "Attached " << pL2->name() << " in node " << pC->name() << " as : " << linkIdL22 << endl;
	cout << endl;
	//
	int nodeIdB3 = pL3->attach(pB);
	cout << "Attached " << pB->name() << " in link " << pL3->name() << " as : " << nodeIdB3 << endl;
	cout << endl;

	int nodeIdD = pL3->attach(pD);
	cout << "Attached " << pD->name() << " in link " << pL3->name() << " as : " << nodeIdD << endl;
	cout << endl;

	int linkIdL3 = pB->attach(pL3);
	cout << "Attached " << pL3->name() << " in node " << pB->name() << " as : " << linkIdL3 << endl;
	cout << endl;

	int linkIdL33 = pD->attach(pL3);
	cout << "Attached " << pL3->name() << " in node " << pD->name() << " as : " << linkIdL33 << endl;
	cout << endl;

	/*int linkIdL2 = pB->attach(pL1);
	cout << "Attached " << pL1->name() << " in node " << pB->name() << " as : " << linkIdL1 << endl;
	cout << endl;*/

	m1["196.168.2.2"] = linkIdL1;

	m2["196.168.2.1"] = linkIdL1;
	m2["196.168.2.3"] = linkIdL2;
	m2["196.168.2.6"] = linkIdL3;

	m3["196.168.2.2"] = linkIdL2;

	m4["196.168.2.2"] = linkIdL3;

	//Call A's, B's, and L's contents() fcns
	/*pA->contents();
	cout << endl;
	pB->contents();
	cout << endl;
	pC->contents();
	cout << endl;
	pD->contents();
	cout << endl;
	pL1->contents();
	cout << endl;
	pL2->contents();
	cout << endl;
	pL3->contents();
	cout << endl;*/


	//Call node A's snd() fcn with the string "10010".	//test 1- entire message in 1 F8 frame
	pA->snd_f8(send_str, linkIdL1, 74);
	cout << endl;

	//for test 2- fragmentation in frame of 30 bytes each
	string str1 = "CPSC 558 -- A";
	string str2 = "dv Networking";
	string str3 = " -- Project P"; 
	string str4 = "art 1 -- Data";
	string str5 = " Link";

	/*pA->snd_f8(str1, linkIdL1, 30);
	pA->snd_f8(str2, linkIdL1, 30);
	pA->snd_f8(str3, linkIdL1, 30);
	pA->snd_f8(str4, linkIdL1, 30);
	pA->snd_f8(str5, linkIdL1, 30);*/

	cout << endl << endl;

	//test 3- sending fragments in reverse order
	/*pA->snd_f8r(str5, linkIdL1, 30);
	pA->snd_f8r(str4, linkIdL1, 30);
	pA->snd_f8r(str3, linkIdL1, 30);
	pA->snd_f8r(str2, linkIdL1, 30);
	pA->snd_f8r(str1, linkIdL1, 30);*/
	
	return 0;
}
