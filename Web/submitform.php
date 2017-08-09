<!DOCTYPE html>
<html>
    <head>
        <title>Changes Submitted</title>
        <?php
            if(file_exists('./parameter.xml')) {
                $doc = new DOMDocument();
                $doc->load('parameter.xml');

                $topNode = new DOMNode;
                $topNode = $doc->childNodes->item(0); //assumes that we only care about the first child (there shouldn't ever be any other top-level children)
            }
        ?>
    </head>

    <body>
        <h1>This will have things later tonight</h1>
        <br/>
        <br/>
        <h3><?php print $_POST['tooHotList']?></h3>
        <h3><?php print $_POST['tooColdList']?></h3>
        <h4>
            <?php
                $nodeList = $topNode->getElementsByTagName('roomRule');
                $len = $nodeList->length;
                for ($i = 0; $i < $len; $i++) {
                    //check to see if it's the node we care about
                    if ($nodeList->item($i)->getAttribute('room') == $_GET['room']) {
                        //check to see if the value changed
                        if ($_POST['tooHotList'] != 0) {
                            $query = "//*[@room = '".$_GET['room']."']";
                            $xp = new DomXPath($doc);
                            $res = $xp->query($query);
                            $res->item(0)->setAttribute('hotCount',$_POST['tooHotList']);
                        }
                        if ($_POST['tooColdList'] != 0) {
                            $query = "//*[@room = '".$_GET['room']."']";
                            $xp = new DomXPath($doc);
                            $res = $xp->query($query);
                            $res->item(0)->setAttribute('coldCount',$_POST['tooColdList']);
                        }

                        print "It worked!";
                    }
                }
                $doc->save('parameter.xml');
            ?>
        </h4>
    </body>
</html>