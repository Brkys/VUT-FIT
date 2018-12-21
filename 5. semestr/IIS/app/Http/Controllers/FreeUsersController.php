<?php

namespace App\Http\Controllers;
use App\userModel;
use App\Invitation;
use Illuminate\Http\Request;

class FreeUsersController extends Controller
{
    public function viewUsers() {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 4){
            return redirect('home');
        }
        else {
            $invitations = Invitation::all();
            $freeUsers = userModel::FreeUser();
            foreach($freeUsers as &$users){
                foreach($invitations as $invitation){
                    if($users['id'] == $invitation->ID_User && $invitation->ID_Familie == $_SESSION['familia']){
                        $users['invited'] = true;
                    }
                }
            }
            unset($users);
            return view('free-users')->with('freeUsers', $freeUsers);
        }
    }

    public function invite(Request $req) {
        session_start();
        $idUzivatele = $req->input('id');
        $invite = new Invitation();
        $invite->ID_User = $idUzivatele;
        $invite->ID_Familie = $_SESSION['familia'];
        $invite->save();
        return redirect('free-users');
    }
}
