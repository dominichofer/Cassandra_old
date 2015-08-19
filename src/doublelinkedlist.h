#pragma once

template <class DataType>
class DoubleLinkedList
{
    class Node
    {
    public:
        DataType data;
        Node *n, *p;

        Node() {}
        Node(DataType data) : data(data) {}

        void push_after(const DataType& data)
        {
            Node * tmp = new Node(data);
            tmp->n = n;
            tmp->p = this;
            n->p = tmp;
            n = tmp;
        }
        void push_before(const DataType& data)
        {
            Node * tmp = new Node(data);
            tmp->n = this;
            tmp->p = p;
            p->n = tmp;
            p = tmp;
        }
    };

	class iterator
	{
		Node *node
	};

    Node BeginEnd;

    DoubleLinkedList()
    {
        BeginEnd.n = &BeginEnd;
        BeginEnd.p = &BeginEnd;
    }

	inline void Detach(Node& node)
	{
		node.p->n = node.n;
		node.n->p = node.p;
		node.p = nullptr;
		node.n = nullptr;
	}
	inline void Remove(Node& node) { Detach(node); delete node; }
	inline void Atach_back(Node& node)
	{
		node.p = BeginEnd.p;
		node.n = &BeginEnd;
		BeginEnd.p->n = &node;
		BeginEnd.p = &node;
	}
	inline void Atach_front(Node& node)
	{
		node.p = &BeginEnd;
		node.n = BeginEnd.n;
		BeginEnd.n->p = &node;
		BeginEnd.n = &node;
	}
	inline void Move_back(Node& node)
	{
		node.p->n = node.n;
		node.n->p = node.p;
		node.p = BeginEnd.p;
		node.n = &BeginEnd;
		BeginEnd.p->n = &node;
		BeginEnd.p = &node;
	}
	inline void Move_front(Node& node)
	{
		node.p->n = node.n;
		node.n->p = node.p;
		node.p = &BeginEnd;
		node.n = BeginEnd.n;
		BeginEnd.n->p = &node;
		BeginEnd.n = &node;
	}
    inline void push_back (const DataType& data) { BeginEnd.push_before(data); }
    inline void push_front(const DataType& data) { BeginEnd.push_after (data); }
    inline Node& begin() { return *BeginEnd.n; }
    inline Node&   end() { return BeginEnd; }
};