<!DOCTYPE>
<html>
    <head></head>

    <body>
        <?php 

            if(file_exists('./parameter.xml')) {
                $doc = new DOMDocument();
                $doc->load('parameter.xml');

                $topNode = new DOMNode;
                $topNode = $doc->childNodes->item(0); //assumes that we only care about the first child (there shouldn't ever be any other top-level children)

                
                /*foreach ($topNode as $qNode) {
                    echo $qNode->getAttribute('display');
                }*/

                assembleChildren($topNode);

                /*$listONodes = new DOMNodeList;
                $listONodes=$doc->childNodes;
                foreach ($listONodes as $element) {
                    print "<p> $element->localName </p><br/>";
                }*/

                

                //$doc->saveXML();
                echo "End of Main";
            }

            else {
                print "<h1>Unable to find XML Document</h1>";
            }


            function assembleChildren($node) {
                removeWhitespace($node);//gets rid of extra nodes that come from the formatting whitespace
                //print '<ul>';//open unordered list tag
                if ($node->hasChildNodes()) {
                    print '<ul>'; //if there are children, there will list items
                    foreach($node->childNodes as $childNode) {
                        $displayAtrb = $childNode->getAttribute('display');
                        if ($displayAtrb != '__false') {
                            print '<li>';
                            print $displayAtrb; //we want to display it
                        }

                        removeWhitespace($childNode);

                        if($childNode->hasChildNodes()) {//if its children have children, call again
                            assembleChildren($childNode);
                        }

                        else {//children don't have children
                            print '</li>';
                        }
                    }
                    print '</ul>';

                }
                else { //node has no children
                    if($node->getAttribute('display') != '__false') {
                        print '</li>';
                    }
                }
                return;
            }

            function removeWhitespace(&$node) {
                $size = $node->childNodes->length;
                for ($i = 0; $i < $size; $i++) {
                    if($node->childNodes[$i]->nodeType == 3 || $node->childNodes[$i]->nodeType == 8) {
                        $node->removeChild($node->childNodes[$i]);
                    }
                }
            }

        ?>
    </body>
</html>