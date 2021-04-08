#pragma once

namespace flexui {

	class Node;

	class TreeScope {
	public:
		TreeScope() { };
		virtual ~TreeScope() { };

		/* Internal calls */
		virtual void _attachedToTree(Node* node) { };
		virtual void _detachedFromTree(Node* node) { };

	protected:

	private:

	};

}