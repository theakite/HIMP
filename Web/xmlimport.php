<!DOCTYPE html>
<html>
    <head>
        <title>HIMP Editor</title>
    </head>

    <body>
    <h1>HIMP Editior</h1>
    <h2>Properties</h2>
        <?php 

            if(file_exists('./parameter.xml')) {
                $doc = new DOMDocument();
                $doc->load('parameter.xml');

                $topNode = new DOMNode;
                $topNode = $doc->childNodes->item(0); //assumes that we only care about the first child (there shouldn't ever be any other top-level children)

                removeWhitespace($topNode);
                assembleChildren($topNode);

                //$doc->saveXML();
                echo "End of File";
            }

            else {
                print "<p>Unable to find XML Document</p>";
            }

            function assembleChildren($node) {
                echo "<h3>Assemble children for: ";
                echo $node->nodeName;
                echo "</h3>";
                //removeWhitespace($node);//gets rid of extra nodes that come from the formatting whitespace
                removeComments($node);
                if ((int)$node->nodeType == 1 && $node->hasChildNodes() == true) {
                    //removeWhitespace($node);
                    print '<ul>'; //if there are children, there will list items;
                    foreach($node->childNodes as $childNode) {
                        echo "<h3>";
                        if ((int)($childNode->nodeType == 1)) {
                            echo "first condition is true ";
                            if($childNode->hasAttribute("display") == true) {
                                echo "second condition is true";
                            }
                            else {
                                echo "second condition is false";
                            }
                        }
                        else {
                            echo "first condition is false";
                        }
                        echo "</h3>";
                        echo "<h3>";
                        
                        echo "</h3>";
                        if ((int)($childNode->nodetype == 1) && ($childNode->hasAttribute("display") == true)) {
                            echo "<h1>IN THE IF STATEMENT</h1>";
                            $displayAtrb = $childNode->getAttribute('display');
                            if ($displayAtrb != '__false') {//TODO: this should move to config file
                                print '<li>';
                                print $displayAtrb; //we want to display it
                            }
                        }

                        else {
                            echo "in the else again!";
                        }

                        //removeWhitespace($childNode);

                        if($childNode->hasChildNodes()) {//if its children have children, call again
                            //removeWhitespace($childNode);
                            assembleChildren($childNode);
                        }

                        else {//children don't have children
                            print '</li>';
                        }
                    }
                    print '</ul>';

                }
                else /*if($node->nodeType == 3) */{ //node has no children
                    echo "no-children elseif";
                    if($node->getAttribute('display') != '__false') {
                        print '<span contenteditable = "true">';
                        print $node->textContent;
                        print '</span></li>';
                    }
                }
                return;
            }

            function removeWhitespace(&$node) {
                if ($node->hasChildNodes()) {
                    $size = $node->childNodes->length;
                    for ($i = 0; $i < $size; $i++) {
                        if(($node->childNodes[$i]->nodeType == 3) || ($node->childNodes[$i]->nodeType == 8)) {
                            $node->removeChild($node->childNodes[$i]);
                        }
                    }
                }
                return;
            }

            function removeComments(&$node) {
                if ($node->hasChildNodes()) {
                    $size = $node->childNodes->length;
                    for ($i = 0; $i < $size; $i++) {
                        if($node->childNodes[$i]->nodeType == 8) {
                            $node->removeChild($node->childNodes[$i]);
                        }
                    }
                }
                return;
            }

        ?>
    </body>
</html>