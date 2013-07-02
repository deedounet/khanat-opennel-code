<?php

function change_info(){
    
 try{
        //if logged in
        if(WebUsers::isLoggedIn()){
            
            if(isset($_POST['target_id'])){
		
                
                if(  ($_POST['target_id'] == $_SESSION['id']) ||  WebUsers::isAdmin()  ){
                    if($_POST['target_id'] == $_SESSION['id']){
                        $target_username = $_SESSION['user'];
                    }else{
                        $target_username = WebUsers::getUsername($_POST['target_id']);
                    }
                    
                    $webUser = new WebUsers();
                    $current_info = $webUser->getInfo($_POST['target_id']);
                    //TODO: XSS filtering
                    $query = "UPDATE ams_user SET ";
                    if(($_POST['FirstName'] != "") && ($_POST['FirstName'] != $current_info['FirstName'])){
                        $query = $query . "FirstName = :fName ";
                    }
                    if(($_POST['LastName'] != "") && ($_POST['LastName'] != $current_info['LastName'])){
                        $query = $query . "LastName = :lName ";
                    }
                    //TODO: add the other fields too
                    $query = $query . "WHERE Login = :user";
                    
                    
                    print($query);
                    exit;
                    
                    
                    
		    $reply = $webUser->checkEmail($_POST['NewEmail']);
		    if ( $reply != "success" ){
			$result['EMAIL_ERROR'] = 'TRUE';
		    }else{
			$result['EMAIL_ERROR'] = 'FALSE';
		    }
		    $result['prevNewEmail'] = $_POST["NewEmail"];
		    
                    if ($reply== "success"){
                        $status = WebUsers::setEmail($target_username, $_POST["NewEmail"] );
                        if($status == 'ok'){
                            $result['SUCCESS_MAIL'] = "OK";
                        }else if($status == 'shardoffline'){
                             $result['SUCCESS_MAIL'] = "SHARDOFF";
                        }
                        $result['permission'] = $_SESSION['permission'];
                        $result['no_visible_elements'] = 'FALSE';
                        $result['target_id'] = $_POST['target_id'];
                        if(isset($_GET['id'])){
                            if(WebUsers::isAdmin() && ($_POST['target_id'] != $_SESSION['id'])){
                                $result['isAdmin'] = "TRUE";
                            }
                        }
                        helpers :: loadtemplate( 'settings', $result);
                        exit;
                         
                    }else{
			$result['EMAIL'] = $reply;
                        $result['permission'] = $_SESSION['permission'];
                        $result['no_visible_elements'] = 'FALSE';
                        $return['username'] = $_SESSION['user'];
                        $result['target_id'] = $_POST['target_id'];
                        if(isset($_GET['id'])){
                            if(WebUsers::isAdmin() && ($_POST['target_id'] != $_SESSION['id'])){
                                $result['isAdmin'] = "TRUE";
                            }
                        }
                        helpers :: loadtemplate( 'settings', $result);
                        exit;
                    }
                    
                }else{
                    //ERROR: permission denied!
                }
        
            }else{
                //ERROR: The form was not filled in correclty
            }    
        }else{
            //ERROR: user is not logged in
            exit;
        }
                  
    }catch (PDOException $e) {
         //go to error page or something, because can't access website db
         print_r($e);
         exit;
    }   
}