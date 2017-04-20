public static class AIPathBuilder {
	/// <summary>
	/// Returns a random path from an enemy spawn to the goal
	/// </summary>
	/// <returns>The enemy path.</returns>
	/// <param name="startNode">Node adjacent to the enemy spawn</param>
	public static Stack<PathNode> createEnemyPath(PathNode startNode){
		/**
		 * This algorthim utilizes a DFS that travels along randomly chosen
		 * adjacent nodes. This creates a random path for an enemy to follow
		 * to the goal.
		 */

		//put the starting PathNode into a TreeNode. This will be the root of the pathing tree
		TreeNode<PathNode> curTreeNode = new TreeNode<PathNode>(startNode);
        
        //Create a stack for the DFS algorithm
		Stack<TreeNode<PathNode>> DFSstack = new Stack<TreeNode<PathNode>>();

        //Create a searched nodes list
        List<PathNode> searchedPathNodes = new List<PathNode>();

        //to be safe, make sure the start node isn't a goal node
        if (startNode.bIsGoal)
        {
			//if the start node is the goal, return a path straight to the goal
			Stack<PathNode> straightToVillage = new Stack<PathNode>();
			straightToVillage.Push(startNode);

			return straightToVillage;
        }
        else
		{//if the start node isn't a goal...
			//Push it into the DFS stack
			DFSstack.Push(curTreeNode);
			//Mark it as searched
			searchedPathNodes.Add(startNode);
        }


        bool bVillageIsFound = false;

		//Begin search
        while (DFSstack.Count > 0 && !bGoalIsFound)
        {
            //Pop a PathNode off the stack
			curTreeNode = DFSstack.Pop();

			if (curTreeNode.storedObject.bIsGoal)
            {
                bGoalIsFound = true;
            }
            else
            {
                //Get unsearched PathNodes adjacent to the current PathNode and randomize their order
				PathNode[] adjNodes = getUnsearchedPathNodes(curTreeNode.storedObject.adjacentNodes, searchedPathNodes);
                
                randomizePathNodeArray(adjNodes);
			
				//Add the unserached adjacent nodes to the DFS Stack
                foreach (PathNode node in adjNodes)
                {
					TreeNode<PathNode> newTreeNode = new TreeNode<PathNode> (node, curTreeNode);
					DFSstack.Push (newTreeNode);
                    searchedPathNodes.Add(node);
                }
            }
        }

        //Print an error if the village hasn't been found yet. 
        if (!bGoalIsFound)
            Debug.LogError("Goal not found; A goal must be added to the level");

		//Create a stack to hold the new path
		Stack<PathNode> newPath = new Stack<PathNode> ();
		//Add PathNodes from the leaf to the root of the pathing tree to the stack
		while (curTreeNode.parent != null) {
			newPath.Push (curTreeNode.storedObject);
			curTreeNode = curTreeNode.parent;
		}
		//Add the PathNode in the root of the pathing tree to the stack
		newPath.Push (curTreeNode.storedObject);

		return newPath;
	}
	
	private static PathNode[] getUnsearchedPathNodes(PathNode[] pathNodeArray, List<PathNode> searchedPathNodes){
		
		//Count the number of unsearched nodes
        int unsearchedPathNodeCount = 0;
        foreach(PathNode node in pathNodeArray)
        {
			if (!searchedPathNodes.Contains(node))
                unsearchedPathNodeCount++;
        }
		
		//Create a new array to hold unsearched nodes
        PathNode[] unsearchedPathNodeArray = new PathNode[unsearchedPathNodeCount];

		//Add all unsearched nodes to the unsearched node array
        int iter = 0;
        foreach(PathNode node in pathNodeArray)
        {
            
			if (!searchedPathNodes.Contains(node))
            {
                unsearchedPathNodeArray[iter] = node;
                iter++;
            }
        }
		
        return unsearchedPathNodeArray;
	}
	
	private static void randomizePathNodeArray(PathNode[] pathNodeArray){
        //Array randomization implemented via Fisher-Yates shuffle
        if (pathNodeArray.Length > 1)
            for(int i=pathNodeArray.Length-1; i > 0; i--)
            {
                int j = Random.Range(0, i+1);
                PathNode temp = pathNodeArray[j];
                pathNodeArray[j] = pathNodeArray[i];
                pathNodeArray[i] = temp;
            }
	}
	
	private class TreeNode<T>{
		/// <summary>
		/// The parent of this TreeNode.
		/// </summary>
		public TreeNode<T> parent;
		/// <summary>
		/// The stored object.
		/// </summary>
		public T storedObject;

		/// <summary>
		/// Initializes a new TreeNode.
		/// </summary>
		/// <param name="storedObject">Stored object for this TreeNode.</param>
		/// <param name="parent">Parent of this TreeNode.</param>
		public TreeNode(T storedObject, TreeNode<T> parent){
			this.storedObject = storedObject;
			this.parent = parent;
		}
		/// <summary>
		/// Initializes a new root TreeNode
		/// </summary>
		/// <param name="storedObject">Stored object of this root TreeNode.</param>
		public TreeNode(T storedObject){
			this.storedObject = storedObject;
		}
	}
}