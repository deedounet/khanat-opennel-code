<?php

class WebUsers extends Users{
    
     /**
     * Function checkUserNameExists
     *
     * @takes $username
     * @return string Info: Returns true or false if the user is in the web db.
     */
     protected function checkUserNameExists($username){
            global $cfg;
            $dbw = new DBLayer($cfg['db']['web']);
            return $dbw->execute("SELECT * FROM ams_user WHERE Login = :name",array('name' => $username))->rowCount();  
     }
    
    
    /**
     * Function checkEmailExists
     *
     * @takes $username
     * @return string Info: Returns true or false if the user is in the www db.
     */
     protected function checkEmailExists($email){
            global $cfg;
            $dbw = new DBLayer($cfg['db']['web']);
            return $dbw->execute("SELECT * FROM ams_user WHERE Email = :email",array('email' => $email))->rowCount();
     }
     
     
    /**
     * Function checkUserPassMatch
     *
     * @takes $username,$password
     * @return string Info: Returns true or false if a login match is found in the web db
     */
     public function checkLoginMatch($username,$password){
        global $cfg;
        
        $dbw = new DBLayer($cfg['db']['web']);
        $statement = $dbw->execute("SELECT * FROM ams_user WHERE Login=:user", array('user' => $username));
        $row = $statement->fetch();
        
        $salt = substr($row['Password'],0,2);
        $hashed_input_pass = crypt($password, $salt);
        if($hashed_input_pass == $row['Password']){
              return $row;
        }else{
              return "fail";
        }	
     }
     
     
    public function getUsername($id){
        global $cfg;
        
        $dbw = new DBLayer($cfg['db']['web']);
        $statement = $dbw->execute("SELECT * FROM ams_user WHERE UId=:id", array('id' => $id));
        $row = $statement->fetch();
        return $row['Login'];
    }
    
    public function isLoggedIn(){
        if(isset($_SESSION['user'])){
            return true;
        }
        return false;
    }
    
    public function isAdmin(){
        if(isset($_SESSION['permission']) && $_SESSION['permission'] == 2){
            return true;
        }
        return false;
    }
    
}