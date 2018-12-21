<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\CProvadiK;
use Illuminate\Support\Facades\DB;

class AssignCriminalController extends Controller
{
    public function assign(){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else {
            if($_SESSION['permission'] < 2 || $_SESSION['permission'] > 4){
                return redirect('no-permission');
            }
            else
            { 
            if($_SESSION['permission'] < 2) {
                return view('no-permission');
            }

            $usersAndPermissions = DB::select("SELECT id, full_name, permission FROM users WHERE familia_id = ".$_SESSION['familia'], [1]);
            $actions = DB::select("SELECT Kriminalni_cinnost.ID_Cinnosti AS ID, TypCinnosti FROM Kriminalni_cinnost INNER JOIN CinnostFamilii ON Kriminalni_cinnost.ID_Cinnosti=CinnostFamilii.ID_Cinnosti WHERE ID_Familie = ".$_SESSION['familia'], [1]);

            return view('assign-criminal')->with('users', $usersAndPermissions)->with('actions', $actions);
            }
        }
    }

    public function assignCriminal(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else if($_SESSION['permission'] < 3 || $_SESSION['permission'] > 4){
            return redirect('no-permission');
        }
        else 
        {
            $newUserAction = new CProvadiK();
            $newUserAction->ID_Clena = $req->input('user_id');
            $newUserAction->ID_Cinnosti = $req->input('action_id');
            $newUserAction->save();
            return redirect('show-criminal');
        }
    }
}
