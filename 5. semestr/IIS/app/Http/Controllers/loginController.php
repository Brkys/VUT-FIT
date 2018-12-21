<?php

namespace App\Http\Controllers;

use App\userModel;
use App\Invitation;
use App\Familie;
use App\newsModel;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;
use Validator;

class loginController extends Controller
{
 
    /**
     * @param Request $req
     * @return view 
     */
    public function login(Request $req)
    {
        $rules = array(
            'username' => 'required|max:255',
            'password' => 'required'
        );
        
        $validatorMessagesCzech = array(
            'username.required' => "Vyplňte, prosím, uživatelské jméno", 
            'password.required' => "Vyplňte, prosím, heslo."
        ); 

        $validator = Validator::make($req->all(), $rules, $validatorMessagesCzech);
        
        if($validator->fails()) {
            return redirect('home')->withErrors($validator)->with('validationFail', true);
        }
        
        $name = $req->input('username');
        $pwd = $req->input('password');
        $found = userModel::FindUser($name, $pwd);
        //uzivtel nalezen
        if($found){
            session_start();
            $_SESSION['loggedIn'] = true;
            $_SESSION['id'] = $found->id;
            $_SESSION['username'] = $found->name;
            $_SESSION['familia'] = $found->familia_id;
            $_SESSION['permission'] = $found->permission;
            return redirect('home');
        }
        //login se nepodaril
        return redirect('home')->with(['openLogin' => true]);
    }

    public function logout(Request $req)
    {
        session_start();
        $_SESSION = array();
        session_destroy();
        return redirect('home')->with(['openLogin' => true]);
    }

    public function account(){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else {
            $myInvites = Invitation::where('ID_User', $_SESSION['id'])->get();
            $invites = array();
            foreach ($myInvites as $key => $value){
                $familie = Familie::where('ID_Familie', $value->ID_Familie)->get();
                if(!empty($familie)){
                    $invite = $familie->toArray();
                    $id = $invite[0]['ID_Familie'];
                    $name = $invite[0]['JmenoFamilie'];
                    $invID = $value->id;
                    array_push($invites, array('ID_Familie' => $id, 'JmenoFamilie' => $name, 'ID_Inv' => $invID));
                }
            }
            $count = DB::select("SELECT COUNT(*) AS count FROM CProvadiK WHERE ID_Clena = ".$_SESSION['id'], [1]);
            $user = DB::select("SELECT name, full_name FROM users WHERE id = ".$_SESSION['id'], [1]);

            return view('account')->with('invites', $invites)->with('count', $count)->with('user', $user);
        }
    }

    public function accept(Request $req){
        //odstranit z invites
        //zmenit permission
        //zmenit familii
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        } else {
            $news = new newsModel();
            $nameFamilia = Familie::where('ID_Familie', $req->input('id'))->get();
            $nameFamilia = $nameFamilia->toArray();
            $nameFamilia = $nameFamilia[0]['JmenoFamilie'];
            $news->date = date("Y-m-d H:i:s");
            $news->title = 'Nový člen';
            

            $user = userModel::find($_SESSION['id']);
            $name = $user->full_name;
            $user->familia_id = $req->input('id');
            $user->permission = 0;
            $user->save();
            $invite = Invitation::find($req->input('invID'));
            $invite->delete();
            $_SESSION['familia'] = $req->input('id');
            $_SESSION['permission'] = 0;

            $news->content = "$name se připojil k familii $nameFamilia.";
            $news->save();
        }
        
        return redirect('news');
    }

    public function decline(Request $req){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        } else {
            $news = new newsModel();
            $nameFamilia = Familie::where('ID_Familie', $req->input('id'))->get();
            $nameFamilia = $nameFamilia->toArray();
            $nameFamilia = $nameFamilia[0]['JmenoFamilie'];
            $news->date = date("Y-m-d H:i:s");
            $news->title = 'Odmítnutí žádosti';
            
            $user = userModel::find($_SESSION['id']);
            $name = $user->full_name;
            $invite = Invitation::find($req->input('invID'));
            $invite->delete();

            $news->content = "$name odmítl pozvání od familie $nameFamilia.";
            $news->save();
        }
        
        return redirect('news');
    }


    public function changeAccount(Request $req){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        } else {
            if($req->input('nickName') != "")
            {
                $test = userModel::where('name', '=', $req->input('nickName'));
                if(!empty($test))
                    return redirect('account');
            }
            $user = userModel::find($_SESSION['id']);
            $user->full_name = empty($req->input('fullName')) ? $user->full_name : $req->input('fullName');
            $user->name = empty($req->input('nickName')) ? $user->name : $req->input('fullName');
            $user->save();
        }
        
        return redirect('account');

    }
}
