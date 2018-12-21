@extends('layouts.app')

@section('title', 'Členové')

@section('navpanel')
    @parent
@endsection

@section('content')
<div class="container" style="padding-top: 100px; width: 80% !important;"> 
    <table class="table table-hover table-dark text-centered" id="custom-table">
        <thead>
            <tr>
                <th scope="col">Jméno</th>
                <th scope="col">Familie</th>
                <th scope="col">Věk</th>
            </tr>
        </thead>
        <tbody>
            @if(!empty($members))
                @foreach($members as $member)
                    <tr>
                        <td>{{$member->Jmeno}}</td>
                        <td>{{$member->JmenoFamilie}}</td>
                        <td>{{$member->Vek}}</td>
                    </tr>
                @endforeach
            @endif
        </tbody>
    </table>
</div>
@endsection