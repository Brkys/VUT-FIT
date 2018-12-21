<?php

namespace App\Http\Controllers;

use App\userModel;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Hash;
use Validator;

class registerController extends Controller
{
 
    /**
     * @param Request $req
     * @return view 
     */
    public function validateRegister(Request $req)
    {
        //pravidla pro finalni validaci
        $rules = array(
            'fullname' => 'required|max:80',
            'date'     => 'date_format:Y-m-d',
            'username' => 'required|max:255',
            'email'    => 'email',
            'password' => 'required|max:255|confirmed'
        );

        //chceme ceske hlasky, delame cesky web
        $validatorMessagesCzech = array(
            'username.required'  => "Vyplňte, prosím, uživatelské jméno.", 
            'password.required'  => "Vyplňte, prosím, heslo.",
            'fullname.required'  => "Vyplňte, prosím, své jméno.",
            'date.date_format'   => "Toto není platný formát data.",
            'email.email'        => "Toto není platný formát e-mailu.",
            'password.confirmed' => "Hesla se neshodují."
        ); 
        
        //jednotlive validace slouzi k nastaveni jednotlivych promennych, ktere neprosly validaci na prazdnou hodnotu
        //je nutne pouzivat pravidla urcena pouze pro hodnotu, kterou validujeme - pri pouziti celeho pole $rules se laravel chova "divne"
        $fullName = $req->input('fullname');
        $validatorFullName = Validator::make($req->only('fullname'), ['fullname' => 'required|max:80'], $validatorMessagesCzech);
        if($validatorFullName->fails()) {
            $fullname = '';
        }

        $date = $req->input('date');
        if($date != ''){
            $validatorDate = Validator::make($req->only('date'), ['date' => 'date_format:Y-m-d'], $validatorMessagesCzech);
            if($validatorDate->fails()) {
                $date = '';
            }
        } else unset($rules['date']); //toto pravidlo dale nebudeme potrebovat a zbytecne by to vypisovalo chybu ohledne formatu data

        $username = $req->input('username');
        $validatorUsername = Validator::make($req->only('username'), ['username' => 'required|max:255'], $validatorMessagesCzech);
        if($validatorUsername->fails()) {
            $username = '';
        }

        $password = $req->input('password');
        $validatorPwd = Validator::make($req->only('password'), ['password' => 'required|max:255|confirmed'], $validatorMessagesCzech);
        if($validatorPwd->fails()) {
            $password = '';
        }

        $email = $req->input('email');
        if($email != ''){
            $validatorEmail = Validator::make($req->only('email'), ['email' => 'email'], $validatorMessagesCzech);
            if($validatorEmail->fails()) {
                $email = '';
            }
        } else unset($rules['email']);

        //naplneni hodnot do pole - dale slouzi ve view pro pripadne doplneni spravne zadanych poli - uzivatel nemusi vse vyplnovat od zacatku
        $validationResult = array(
            'fullname' => $fullName,
            'username' => $username, 
            'date'     => $date,
            'password' => $password,
            'email'    => $email);
        
        //finalni validace pouzita pouze kvuli generovani erroru nesplneni danych pravidel (proto predesle odstraneni pravidel prazdnych poli)
        $validatorFinal = Validator::make($req->all(), $rules, $validatorMessagesCzech);
        if($validatorFinal->fails()) {
            return redirect('home')->withErrors($validatorFinal)->with('registerNotValid', true)->with('registerInput', $validationResult);
        }

        
        $userExists = userModel::where('name', $req->input('username'))->count();
        if($userExists !== 0){
            return redirect('home')->with('registerNotValid', true)->with('registerInput', $validationResult)->with('userExists', true);
        }
        
        return $this->register($req);
    }

    public function register(Request $req){
        //ulozeni noveho uzivatele do databaze
        $newUser = new userModel();
        $newUser->name = $req->input('username');
        $newUser->full_name = $req->input('fullname');
        $newUser->birth_date = $req->input('date');
        $newUser->email = $req->input('email');
        $newUser->password = Hash::make($req->input('password'));
        $newUser->save();

        //prihlaseni noveho uzivatele
        session_start();
        $_SESSION['loggedIn'] = true;
        $_SESSION['id'] = $newUser->id;
        $_SESSION['username'] = $newUser->name;
        $_SESSION['permission'] = $newUser->permission;

        //zaverecne presmerovani
        return redirect('home');
    }
}
