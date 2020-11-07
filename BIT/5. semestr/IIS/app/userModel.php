<?php

namespace App;

use Illuminate\Support\Facades\Hash;
use Illuminate\Database\Eloquent\Model;

class userModel extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'users';

    /**
     * @param Builder $query - nutne jako prvni parametr - predava se automaticky
     * @param string $name
     * @param string $pwd
     * @return mixed $found
     */
    public function scopeFindUser($query, $name, $pwd){
        $found = false;
        $result = $this::all();
        foreach ($result as $key => $value) {
            if($value->name == $name && Hash::check($pwd, $value->password)){
                $found = $value;
                break;
            }
        }
        return $found;
    }

    /**
     * 
     */
    public function scopeFreeUser($query){
        $freeUsers = array();
        $result = $this::where('permission', -1)->get();
        foreach ($result as $key => $value) {
            $user = array('name' => $value->name, 'fullname' => $value->full_name, 'id' => $value->id, 'invited' => false);
            array_push($freeUsers, $user);
        }
        return $freeUsers;
    }
}
